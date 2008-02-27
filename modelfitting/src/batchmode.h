#ifndef BATCHMODE_H
#define BATCHMODE_H

#include <QStringList>

#include "frameinfo.h"

class QSignalMapper;

class BatchMode
{
public:
	BatchMode();
	~BatchMode();
	
	int exec();

private:
	bool parseFileList();
	void waitForOperations(const QList<MapReduceOperation>& operations);
	
	QStringList m_files;
	QSignalMapper* m_sigMapper;
};

#endif
