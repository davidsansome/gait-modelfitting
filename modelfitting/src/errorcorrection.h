#ifndef ERRORCORRECTION_H
#define ERRORCORRECTION_H

#include "ui_errorcorrection.h"

#include <QDialog>
#include <QSettings>

class FrameSet;


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
	void initData();
	float energy(float period, float phase);
	
	Ui_ErrorCorrection m_ui;
	
	QSettings m_settings;
	
	FrameSet* m_frameSet;
	int m_dataSize;
	float* m_data;
	float* m_dataEnd;
};

#endif
