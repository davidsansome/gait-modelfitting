#include "batchmode.h"
#include "frameset.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QSignalMapper>
#include <QDir>

#include <iostream>

BatchMode::BatchMode()
	: m_frameSet(NULL)
{
}

BatchMode::~BatchMode()
{
	delete m_frameSet;
}

int BatchMode::exec()
{
	if (!parseFileList())
		return 1;
	
	for (int i=0 ; i<m_frameSet->count() ; ++i)
	{
		std::cout << "Processing " << m_frameSet->name(i).toAscii().data() << std::endl;
		
		FrameInfo* info = m_frameSet->loadFrame(i);
		std::cout << "    - Loaded mesh" << std::endl;
		if (info->hasModelInformation())
			std::cout << "    - Frame already has model information, recalculating" << std::endl;
		
		waitForOperations(info->update());
		
		std::cout << "    - Finished calculating" << std::endl;
		
		delete info;
	}
	
	return 0;
}

bool BatchMode::parseFileList()
{
	QStringList args(QCoreApplication::arguments());
	args.removeFirst();
	
	foreach (QString arg, args)
	{
		QFileInfo info(arg);
		
		if (!info.exists())
		{
			std::cerr << arg.toAscii().data() << ": file does not exist" << std::endl;
			return false;
		}
		if (info.isDir())
		{
			std::cout << "Using directory: " << arg.toAscii().data() << std::endl;
			m_frameSet = new FrameSet(arg);
			return true;
		}
		else
		{
			std::cerr << arg.toAscii().data() << ": is not a directory" << std::endl;
			return false;
		}
	}
	return false;
}

void BatchMode::waitForOperations(const QList<MapReduceOperation>& operations)
{
	foreach (MapReduceOperation operation, operations)
		operation.second.waitForFinished();
}

