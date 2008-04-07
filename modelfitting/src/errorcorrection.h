#ifndef ERRORCORRECTION_H
#define ERRORCORRECTION_H

#include "ui_errorcorrection.h"

#include <QDialog>
#include <QSettings>
#include <QtDebug>
#include <limits>

class FrameSet;
class CurveFitter;


class ErrorCorrection : public QDialog
{
	Q_OBJECT
public:
	ErrorCorrection(QWidget* parent = 0);
	~ErrorCorrection();
	
	void setFrameSet(FrameSet* frameSet);
	
private slots:
	void okClicked();

private:
	Ui_ErrorCorrection m_ui;
	QSettings m_settings;
	
	CurveFitter* m_curveFitter;
};

#endif
