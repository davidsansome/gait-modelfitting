#include <QApplication>

#include "engine.h"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	Engine engine;
	engine.show();
	engine.setImage("/home/david/Dog-Fluffy_Destroyer_of_worlds.png");
	engine.loadShaders("/home/david/vert.cg", "/home/david/frag.cg");
	engine.setShadersEnabled(true);

	return app.exec();
}

