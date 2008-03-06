#ifndef ERRORCORRECTION_H
#define ERRORCORRECTION_H

#include "ui_errorcorrection.h"

#include <QDialog>


class ErrorCorrection : public QDialog
{
	Q_OBJECT

public:
	ErrorCorrection(QWidget* parent = 0);
	
private slots:
	void okClicked();

private:
	Ui_ErrorCorrection m_ui;
};

#endif
