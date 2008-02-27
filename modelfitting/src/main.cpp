#include <QApplication>
#include <QtDebug>
#include <QFile>

#include <iostream>

#include "mainwindow.h"
#include "glwin.h"
#include "batchmode.h"

void printHelp(const char* appName)
{
	std::cout << "Usage: " << appName << " [options] [voxelspace files]" << std::endl;
	std::cout << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "    -h  --help    Displays this message" << std::endl;
	std::cout << std::endl;
	std::cout << "If any filenames are specified on the commandline the GUI will not be shown and" << std::endl;
	std::cout << "the application will operate in batch mode.  Each of the files specified will" << std::endl;
	std::cout << "be processed and a corresponding .info file produced." << std::endl;
}

int main(int argc, char** argv)
{
	// Work out if we should be using the GUI
	bool gui = getenv("DISPLAY") != 0;
	bool filesOnCommandLine = false;
	for (int i=1 ; i<argc ; ++i)
	{
		QString arg(argv[i]);
		
		if (arg == "-h" || arg == "--help")
		{
			printHelp(argv[0]);
			return 0;
		}
		filesOnCommandLine = true;
	}
	
	if (!gui && !filesOnCommandLine)
	{
		printHelp(argv[0]);
		return 0;
	}
	if (filesOnCommandLine)
		gui = false;
	
	
	QApplication app(argc, argv, gui);
	
	QCoreApplication::setOrganizationName("ECS");
	QCoreApplication::setOrganizationDomain("ecs.soton.ac.uk");
	QCoreApplication::setApplicationName("Modelfitting");
	
	if (gui)
	{
		MainWindow win;
		win.show();
		
		return app.exec();
	}
	else
	{
		BatchMode batch;
		return batch.exec();
	}
}

