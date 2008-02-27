#include "batchmode.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QSignalMapper>

#include <iostream>

BatchMode::BatchMode()
{
}

BatchMode::~BatchMode()
{
}

int BatchMode::exec()
{
	if (!parseFileList())
		return 1;
	
	foreach (QString file, m_files)
	{
		std::cout << "Processing " << file.toAscii().data() << std::endl;
		
		FrameInfo* info = new FrameInfo(file);
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
		if (!info.isFile())
		{
			std::cerr << arg.toAscii().data() << ": is not a file" << std::endl;
			return false;
		}
		
		m_files << arg;
	}
	return true;
}

void BatchMode::waitForOperations(const QList<MapReduceOperation>& operations)
{
	foreach (MapReduceOperation operation, operations)
		operation.second.waitForFinished();
}

