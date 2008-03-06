#ifndef BATCHMODE_H
#define BATCHMODE_H

#include <QStringList>

#include "frameinfo.h"

class QSignalMapper;
class FrameSet;

class BatchMode
{
public:
	BatchMode();
	~BatchMode();
	
	int exec();

private:
	bool parseFileList();
	void waitForOperations(const QList<MapReduceOperation>& operations);
	
	FrameSet* m_frameSet;
	QSignalMapper* m_sigMapper;
};

#endif
