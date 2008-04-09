#ifndef ERRORCORRECTION_H
#define ERRORCORRECTION_H

#include "ui_errorcorrection.h"

#include <QDialog>
#include <QSettings>
#include <QtDebug>
#include <QModelIndex>
#include <limits>

class CurveFitter;
class FrameModel;
class FrameModelFilter;


class ErrorCorrection : public QDialog
{
	Q_OBJECT
public:
	ErrorCorrection(FrameModel* model, QWidget* parent = 0);
	~ErrorCorrection();
	
	void setFrameSet(const QModelIndex& index);
	
private slots:
	void okClicked();

private:
	Ui_ErrorCorrection m_ui;
	QSettings m_settings;
	
	CurveFitter* m_curveFitter;
	
	FrameModel* m_model;
	FrameModelFilter* m_filter;
};

#endif
