#include "glview.h"

GLView::GLView(QWidget* parent)
	: QGLWidget(parent)
{
}

GLView::~GLView()
{
}

void GLView::resizeGL(int width, int height)
{
}

void GLView::initializeGL()
{
}

void GLView::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
