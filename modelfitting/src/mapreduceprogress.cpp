#include "mapreduceprogress.h"

#include <QStandardItemModel>
#include <QAbstractItemDelegate>
#include <QPainter>
#include <QStyle>
#include <QApplication>
#include <QFutureWatcher>
#include <QCloseEvent>
#include <QMessageBox>

class ProgressDelegate : public QAbstractItemDelegate
{
public:
	ProgressDelegate(QObject* parent = 0) : QAbstractItemDelegate(parent), m_style(QApplication::style()) {}
	~ProgressDelegate() {}
	
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
	QStyle* m_style;
};

void ProgressDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QFuture<void> future(index.data(Qt::DisplayRole).value<QFuture<void> >());
	
	QStyleOptionProgressBarV2 progressOption;
	progressOption.minimum = future.progressMinimum();
	progressOption.maximum = future.progressMaximum();
	progressOption.progress = future.progressValue();
	progressOption.orientation = Qt::Horizontal;
	progressOption.state = QStyle::State_Enabled | QStyle::State_Horizontal;
	progressOption.direction = option.direction;
	progressOption.fontMetrics = option.fontMetrics;
	progressOption.palette = option.palette;
	progressOption.rect = option.rect;
	
	m_style->drawControl(QStyle::CE_ProgressBar, &progressOption, painter);
}

QSize ProgressDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return QSize(100, 25);
}



MapReduceProgress::MapReduceProgress(QWidget* parent)
	: QDialog(parent)
{
	m_ui.setupUi(this);
	
	m_model = new QStandardItemModel(this);
	m_model->setColumnCount(2);
	m_ui.view->setModel(m_model);
	m_ui.view->setItemDelegateForColumn(1, new ProgressDelegate(this));
	
	connect(m_ui.cancelButton, SIGNAL(clicked()), SLOT(close()));
}

void MapReduceProgress::addOperation(const MapReduceOperation& op)
{
	QStandardItem* label = new QStandardItem(op.first);
	QStandardItem* prog = new QStandardItem();
	prog->setData(QVariant::fromValue(op.second), Qt::DisplayRole);
	
	m_model->appendRow(QList<QStandardItem*>() << label << prog);
	
	QFutureWatcher<void>* watcher = new QFutureWatcher<void>(this);
	connect(watcher, SIGNAL(progressRangeChanged(int, int)), SLOT(operationChanged()));
	connect(watcher, SIGNAL(progressValueChanged(int)), SLOT(operationChanged()));
	connect(watcher, SIGNAL(finished()), SLOT(operationFinished()));
	watcher->setFuture(op.second);
}

void MapReduceProgress::addOperations(const QList<MapReduceOperation>& ops)
{
	foreach (MapReduceOperation op, ops)
		addOperation(op);
}

QStandardItem* MapReduceProgress::itemForFuture(const QFuture<void>& future)
{
	for (int i=0 ; i<m_model->rowCount() ; ++i)
	{
		QStandardItem* item = m_model->item(i, 1);
		QFuture<void> itemFuture(item->data(Qt::DisplayRole).value<QFuture<void> >());
		if (future == itemFuture)
			return item;
	}
	return NULL;
}

void MapReduceProgress::operationChanged()
{
	QFutureWatcher<void>* watcher = (QFutureWatcher<void>*) sender();
	
	m_ui.view->update(itemForFuture(watcher->future())->index());
}

void MapReduceProgress::operationFinished()
{
	QFutureWatcher<void>* watcher = (QFutureWatcher<void>*) sender();
	
	m_model->removeRow(itemForFuture(watcher->future())->row());
	watcher->deleteLater();
	
	if (m_model->rowCount() == 0)
		accept();
}

void MapReduceProgress::closeEvent(QCloseEvent* event)
{
	if (QMessageBox::question(this, "Cancel all?", "Are you sure you want to cancel all remaining mapreduce operations?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
	{
		for (int i=0 ; i<m_model->rowCount() ; ++i)
		{
			QStandardItem* item = m_model->item(i, 1);
			QFuture<void> itemFuture(item->data(Qt::DisplayRole).value<QFuture<void> >());
			itemFuture.cancel();
		}
		event->accept();
	}
	else
		event->ignore();
}

