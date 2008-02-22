#ifndef MAPREDUCEPROGRESS_H
#define MAPREDUCEPROGRESS_H

#include <QDialog>

#include "ui_mapreduceprogress.h"
#include "frameinfo.h"

class QStandardItemModel;
class QStandardItem;
class QCloseEvent;

class MapReduceProgress : public QDialog
{
	Q_OBJECT
public:
	MapReduceProgress(QWidget* parent = 0);
	
	void closeEvent(QCloseEvent* event);
	
public slots:
	void addOperation(const MapReduceOperation& op);
	void addOperations(const QList<MapReduceOperation>& ops);
	
	void reject() {close();}

private slots:
	void operationChanged();
	void operationFinished();

private:
	QStandardItem* itemForFuture(const QFuture<void>& future);
	
	Ui_MapReduceProgress m_ui;
	
	QStandardItemModel* m_model;
};

#endif
