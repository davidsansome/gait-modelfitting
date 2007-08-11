#include "engine.h"

#include <QImage>

Engine::Engine()
	: QGLWidget(),
	  texture(0)
{
}

Engine::~Engine()
{
	glDeleteTextures(1, &texture);
}

void Engine::setImage(const QString& fileName)
{
	glDeleteTextures(1, &texture);
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	QImage image(fileName);
	int numBytes = image.numBytes();
	uchar* bits = image.bits();
	
	// Convert ARGB to RGBA
	for (uchar* p=bits ; p<bits+numBytes ; p+=4)
	{
		QRgb color = *((QRgb*)p);
		*(p+0) = qRed(color);
		*(p+1) = qGreen(color);
		*(p+2) = qBlue(color);
		*(p+3) = qAlpha(color);
	}

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA8, image.width(), image.height(), GL_RGBA, GL_UNSIGNED_BYTE, image.bits());

	resize(image.width(), image.height());
}

void Engine::initializeGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_TEXTURE_2D);
}

void Engine::resizeGL(int w, int h)
{
	glViewport(0, 0, width(), height());

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0f, 1.0f, 1.0f, 0.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Engine::paintGL()
{
	if (texture != 0)
		glBindTexture(GL_TEXTURE_2D, texture);

	
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(0.0f, 0.0f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(1.0f, 0.0f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(1.0f, 1.0f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(0.0f, 1.0f);
	glEnd();
}

