#include <QApplication>

#include "engine.h"
#include "controller.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	Engine engine;
	Controller controller(&engine);
	
	engine.show();
	controller.show();

	return app.exec();
}

