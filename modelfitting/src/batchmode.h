#ifndef BATCHMODE_H
#define BATCHMODE_H

#include <QStringList>
#include <QObject>
#include <QQueue>

#include "frameinfo.h"

class QSignalMapper;
class FrameSet;

typedef QPair<FrameSet*, int> QueuedFrame;

class BatchMode : public QObject
{
	Q_OBJECT
public:
	BatchMode();
	~BatchMode();
	
	void setJobs(int jobs) { m_jobs = jobs; }
	
	bool start();

private slots:
	void futureFinished(int id);

private:
	bool parseFileList();
	bool startNextJob();
	
	QList<FrameSet*> m_frameSets;
	QQueue<QueuedFrame> m_queue;
	
	QSignalMapper* m_signalMapper;
	QList<QPair<QString, FrameInfo*> > m_activeOperations;
	
	int m_jobs;
	int m_jobsRunning;
};

#endif
