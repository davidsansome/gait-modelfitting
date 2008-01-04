#include <QApplication>

#include "contour.h"

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	
	Contour c("../input.png", NULL);
	c.show();
	
	return app.exec();
}

