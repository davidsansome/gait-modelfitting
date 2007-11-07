#include <QApplication>

#include "mainwindow.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	
	QCoreApplication::setOrganizationName("ECS");
	QCoreApplication::setOrganizationDomain("ecs.soton.ac.uk");
	QCoreApplication::setApplicationName("Modelfitting");
	
	MainWindow win;
	win.show();
	
	return app.exec();
}

