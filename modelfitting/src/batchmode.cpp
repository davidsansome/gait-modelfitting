#include "batchmode.h"
#include "frameset.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QSignalMapper>
#include <QDir>
#include <QFutureWatcher>

#include <iostream>

BatchMode::BatchMode()
	: m_jobs(1),
	  m_jobsRunning(0)
{
	m_signalMapper = new QSignalMapper(this);
	connect(m_signalMapper, SIGNAL(mapped(int)), SLOT(futureFinished(int)));
}

BatchMode::~BatchMode()
{
	foreach (FrameSet* s, m_frameSets)
		delete s;
}

bool BatchMode::start()
{
	if (!parseFileList())
		return false;
	
	for (int i=0 ; i<m_jobs ; ++i)
	{
		if (!startNextJob())
			break;
	}
	
	return true;
}

bool BatchMode::startNextJob()
{
	if (m_queue.isEmpty())
		return false;
	
	QueuedFrame frame(m_queue.dequeue());
	QString name(frame.first->directory() + "/" + frame.first->name(frame.second));
	
	std::cout << name.toUtf8().data() << ": Starting..." << std::endl;
	
	FrameInfo* info = frame.first->loadFrame(frame.second);
	if (info->hasModelInformation())
		std::cout << name.toUtf8().data() << ": Frame already has model information - recalculating" << std::endl;
	
	QList<MapReduceOperation> ops(info->update());
	foreach (MapReduceOperation op, ops)
	{
		QFutureWatcher<void>* watcher = new QFutureWatcher<void>(this);
		watcher->setFuture(op.second);
		connect(watcher, SIGNAL(finished()), m_signalMapper, SLOT(map()));
		m_signalMapper->setMapping(watcher, m_activeOperations.count());
		
		m_activeOperations << QPair<QString, FrameInfo*>(op.first, info);
	}
	m_jobsRunning++;
	return true;
}

void BatchMode::futureFinished(int id)
{
	QString operationName(m_activeOperations[id].first);
	FrameInfo* info = m_activeOperations[id].second;
	
	std::cout << info->filename().toUtf8().data() << ": " << operationName.toUtf8().data() << " finished" << std::endl;
	
	if (info->hasModelInformation())
	{
		info->save();
		delete info;
		m_jobsRunning --;
		
		startNextJob();
		
		if (m_jobsRunning == 0)
			QCoreApplication::exit(0);
	}
}

bool BatchMode::parseFileList()
{
	QStringList args(QCoreApplication::arguments());
	args.removeFirst();
	
	for (int i=0 ; i<args.count() ; ++i)
	{
		QString arg(args[i]);
		QFileInfo info(arg);
		
		if (arg == "-j")
		{
			bool ok = false;
			m_jobs = args[++i].toInt(&ok);
			
			if (!ok)
			{
				std::cerr << "The number of jobs must be an integer" << std::endl;
				return false;
			}
			continue;
		}
		else if (arg.startsWith("--jobs="))
		{
			bool ok = false;
			m_jobs = arg.right(arg.length() - 7).toInt(&ok);
			
			if (!ok)
			{
				std::cerr << "The number of jobs must be an integer" << std::endl;
				return false;
			}
			continue;
		}
		else if (!info.exists())
		{
			std::cerr << arg.toAscii().data() << ": file does not exist" << std::endl;
			return false;
		}
		else if (info.isDir())
		{
			std::cout << "Queueing all files in directory: " << arg.toAscii().data() << std::endl;
			FrameSet* frameSet = new FrameSet(arg);
			m_frameSets << frameSet;
			
			for (int i=0 ; i<frameSet->count() ; ++i)
				m_queue << QueuedFrame(frameSet, i);
		}
		else
		{
			std::cerr << arg.toAscii().data() << ": is not a directory" << std::endl;
			return false;
		}
	}
	return m_queue.count() > 0;
}

