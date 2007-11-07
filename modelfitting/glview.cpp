#include "glview.h"

QGLWidget* GLView::s_contextWidget = NULL;

GLView::GLView(QWidget* parent)
	: QGLWidget(parent, s_contextWidget),
	  m_mesh(NULL),
	  m_viewType(Front),
	  m_zoom(1.0)
{
	if (!s_contextWidget)
		s_contextWidget = this;
}

GLView::~GLView()
{
}

void GLView::setMesh(Mesh* mesh)
{
	m_mesh = mesh;
}

void GLView::setViewType(ViewType type)
{
	m_viewType = type;
}

void GLView::initializeGL()
{
	glEnable(GL_DEPTH_TEST);
}

void GLView::resizeGL(int width, int height)
{
	glViewport(0, 0, width, height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, float(width) / height, 1.0, 2000.0);
	
	glMatrixMode(GL_MODELVIEW);
}

void GLView::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	switch (m_viewType)
	{
		case Front:    gluLookAt(0.0, 400.0 * m_zoom, 100.0, 0.0, 0.0, 100.0, 0.0, 0.0, 1.0);            break;
		case Side:     gluLookAt(400.0 * m_zoom, 0.0, 100.0, 0.0, 0.0, 100.0, 0.0, 0.0, 1.0);            break;
		case Overhead: gluLookAt(0.0, 0.0, 500.0 * m_zoom, 0.0, 0.0, 100.0, -1.0, 0.0, 0.0);             break;
		case Angle:    gluLookAt(283.0 * m_zoom, 283.0 * m_zoom, 100.0, 0.0, 0.0, 100.0, 0.0, 0.0, 1.0); break; // 400 * sin(pi/4) = 283
	}
	
	if (!m_mesh)
		return;
	
	m_mesh->draw();
}
