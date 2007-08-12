#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QDialog>
#include "ui_controller.h"

class Engine;

class Controller : public QDialog
{
	Q_OBJECT
public:
	Controller(Engine* engine);
	~Controller();

private slots:
	void openFile();
	void filterChanged(int filter);

private:
	Ui_Controller m_ui;
	Engine* m_engine;
};

#endif
