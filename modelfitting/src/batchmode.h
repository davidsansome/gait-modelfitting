#ifndef BATCHMODE_H
#define BATCHMODE_H

#include <QStringList>
#include <QObject>
#include <QQueue>

#include "frameinfo.h"

class QSignalMapper;
class FrameModel;
class FrameModelFilter;

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
	
	FrameModel* m_model;
	FrameModelFilter* m_frameFilter;
	
	QQueue<QModelIndex> m_queue;
	
	QSignalMapper* m_signalMapper;
	QList<QPair<QString, FrameInfo*> > m_activeOperations;
	
	int m_jobs;
	int m_jobsRunning;
};

#endif
