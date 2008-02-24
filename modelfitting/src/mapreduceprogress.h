#ifndef MAPREDUCEPROGRESS_H
#define MAPREDUCEPROGRESS_H

#include <QDialog>
#include <QTime>

#include "ui_mapreduceprogress.h"
#include "frameinfo.h"

class QStandardItemModel;
class QStandardItem;
class QCloseEvent;
class QShowEvent;
class QTimerEvent;

class MapReduceProgress : public QDialog
{
	Q_OBJECT
public:
	MapReduceProgress(QWidget* parent = 0);
	
public slots:
	void addOperation(const MapReduceOperation& op);
	void addOperations(const QList<MapReduceOperation>& ops);
	
	void reject() {close();}

protected:
	void closeEvent(QCloseEvent* event);
	void showEvent(QShowEvent* event);
	void hideEvent(QShowEvent* event);
	void timerEvent(QTimerEvent* event);

private slots:
	void operationChanged();
	void operationFinished();

private:
	QStandardItem* itemForFuture(const QFuture<void>& future);
	QString timeComponent(int* ms, int denom, const QString& unit, bool alwaysShow = false) const;
	void updateTimeElapsed();
	
	Ui_MapReduceProgress m_ui;
	
	QStandardItemModel* m_model;
	QTime m_timeElapsed;
	int m_timerId;
};

#endif
