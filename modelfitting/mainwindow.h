#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>

#include "ui_mainwindow.h"

class MainWindow : public QDialog
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow();
	
private:
	Ui_MainWindow m_ui;
};

#endif
