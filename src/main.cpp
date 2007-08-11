#include <QApplication>

#include "engine.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	Engine engine;
	engine.show();
	engine.setImage("/home/david/grid.png");

	return app.exec();
}

