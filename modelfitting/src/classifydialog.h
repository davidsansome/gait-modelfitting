#ifndef CLASSIFYDIALOG_H
#define CLASSIFYDIALOG_H

#include <QDialog>

#include "ui_classifydialog.h"

class FrameModel;

class PersonItem : public QListWidgetItem
{
public:
	PersonItem(const QString& c, double d, QListWidget* parent);
	
	QString classification() const { return m_classification; }
	double distance() const { return m_distance; }

private:
	QString m_classification;
	double m_distance;
};


class ClassifyDialog : public QDialog
{
	Q_OBJECT
public:
	ClassifyDialog(FrameModel* model, QWidget* parent = 0);
	~ClassifyDialog();
	
	void setFrameSet(const QModelIndex& frameSet);

private slots:
	void okClicked();

private:
	double distanceTo(const QModelIndex& other) const;
	
	Ui_ClassifyDialog m_ui;
	
	QModelIndex m_index;
	FrameModel* m_model;
};

#endif
