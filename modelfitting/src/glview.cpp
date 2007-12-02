#include "glview.h"
#include "frameinfo.h"
#include "convolution.h"

#include <QDebug>

QGLWidget* GLView::s_contextWidget = NULL;

GLView::GLView(QWidget* parent)
	: QGLWidget(parent, s_contextWidget),
	  m_mesh(NULL),
	  m_frameInfo(NULL),
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
	m_frameInfo = NULL;
}

void GLView::setViewType(ViewType type)
{
	m_viewType = type;
}

void GLView::initializeGL()
{
	glEnable(GL_NORMALIZE);
	//glEnable(GL_CULL_FACE);
	
	// Lighting
	static GLfloat pos1[4] = {500.0, -1000.0, 300.0, 1.0};
	static GLfloat amb[4] = { 0.2, 0.2, 0.2, 1.0 };
	static GLfloat diff[4] = { 0.7, 0.7, 0.7, 1.0 };
	static GLfloat spec[4] = { 1.0, 1.0, 1.0, 1.0 };
	static GLfloat gamb[4] = { 0.5, 0.5, 0.5, 1.0 };
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec);
	glLightfv(GL_LIGHT0, GL_POSITION, pos1);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gamb);
	glEnable(GL_LIGHT0);
	
	m_quadric = gluNewQuadric();
}

#define MAXX 38.0
#define MINX (-MAXX)
#define MAXY 183.0
#define MINY (-MAXY)
#define MAXZ 100.0
#define MINZ (-MAXZ)

#define XEXTENT (MAXX*2)
#define YEXTENT (MAXY*2)
#define ZEXTENT (MAXZ*2)

void GLView::resizeGL(int width, int height)
{
	float aspectRatio = float(width) / height;
	glViewport(0, 0, width, height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	const float zNear = 1.0;
	const float zFar = 2000.0;
	
	if (m_viewType == Angle)
		gluPerspective(45.0, aspectRatio, zNear, zFar);
	else
	{
		float x, y;
		switch (m_viewType)
		{
			case Front:    x = XEXTENT; y = ZEXTENT; break;
			case Side:     x = YEXTENT; y = ZEXTENT; break;
			case Overhead: x = YEXTENT; y = XEXTENT; break;
		}
		
		if (aspectRatio < x/y)
		{
			float scale = x/width;
			glOrtho(-x/2.0, x/2.0, -scale*height/2.0, scale*height/2.0, zNear, zFar);
		}
		else
		{
			float scale = y/height;
			glOrtho(-scale*width/2.0, scale*width/2.0, -y/2.0, y/2.0, zNear, zFar);
		}
	}
	
	glMatrixMode(GL_MODELVIEW);
}

void GLView::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	
	switch (m_viewType)
	{
		case Front:    gluLookAt(0.0, 200.0, 100.0, 0.0, 0.0, 100.0, 0.0,  0.0, 1.0);             break;
		case Side:     gluLookAt(200.0, 0.0, 100.0, 0.0, 0.0, 100.0, 0.0,  0.0, 1.0);             break;
		case Overhead: gluLookAt(0.0, 0.0, 200.0,   0.0, 0.0, 100.0, -1.0, 0.0, 0.0);             break;
		case Angle:    gluLookAt(283.0 * m_zoom - 50.0, 283.0 * m_zoom, 100.0, -50.0, 0.0, 100.0, 0.0, 0.0, 1.0); break; // 400 * sin(pi/4) = 283
	}
	
	if (!m_mesh)
		return;
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	m_mesh->draw();
	glDisable(GL_LIGHTING);
	
	drawTunnel();
	
	/*glPushMatrix();
		glTranslatef(0.0, 0.0, 100.0);
		glScalef(50.0, 50.0, 50.0);
		drawTestCube();
	glPopMatrix();*/
	
	glDisable(GL_DEPTH_TEST);
	drawInfo();
}

void GLView::drawTunnel()
{
	// TODO: Use the #defines above
	static const float xExtent = 38.0;
	static const float yExtent = 183.0;
	static const float zExtent = 200.0;
	
	glColor4f(0.0, 1.0, 0.0, 1.0);
	
	// Left side
	glBegin(GL_LINE_LOOP);
		glVertex3f(-xExtent, -yExtent, 0.0);
		glVertex3f(-xExtent, yExtent, 0.0);
		glVertex3f(-xExtent, yExtent, zExtent);
		glVertex3f(-xExtent, -yExtent, zExtent);
	glEnd();
	
	// Right side
	glBegin(GL_LINE_LOOP);
		glVertex3f(xExtent, -yExtent, 0.0);
		glVertex3f(xExtent, yExtent, 0.0);
		glVertex3f(xExtent, yExtent, zExtent);
		glVertex3f(xExtent, -yExtent, zExtent);
	glEnd();
	
	// Lines going across
	glBegin(GL_LINES);
		glVertex3f(-xExtent, yExtent, 0.0);
		glVertex3f(xExtent, yExtent, 0.0);
		glVertex3f(-xExtent, -yExtent, 0.0);
		glVertex3f(xExtent, -yExtent, 0.0);
		glVertex3f(-xExtent, yExtent, zExtent);
		glVertex3f(xExtent, yExtent, zExtent);
		glVertex3f(-xExtent, -yExtent, zExtent);
		glVertex3f(xExtent, -yExtent, zExtent);
	glEnd();
}

void GLView::drawTestCube()
{
	glBegin(GL_QUADS);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-0.5f, 0.5f, -0.5f);
		glVertex3f(0.5f, 0.5f, -0.5f);
		glVertex3f(0.5f, 0.5f, 0.5f);
		glVertex3f(-0.5f, 0.5f, 0.5f);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.5f, 0.5f, 0.5f);
		glVertex3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, 0.5f);
		glVertex3f(-0.5f, -0.5f, 0.5f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.5f, -0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, -0.5f);
		glVertex3f(-0.5f, -0.5f, -0.5f);

		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(-0.5f, -0.5f, -0.5f);
		glVertex3f(0.5f, -0.5f, -0.5f);
		glVertex3f(0.5f, 0.5f, -0.5f);
		glVertex3f(-0.5f, 0.5f, -0.5f);

		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(-0.5f, 0.5f, -0.5f);
		glVertex3f(-0.5f, 0.5f, 0.5f);
		glVertex3f(-0.5f, -0.5f, 0.5f);
		glVertex3f(-0.5f, -0.5f, -0.5f);

		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.5f, 0.5f, -0.5f);
		glVertex3f(0.5f, -0.5f, -0.5f);
		glVertex3f(0.5f, -0.5f, 0.5f);
	glEnd();
}

void GLView::drawInfo()
{
	if (m_frameInfo == NULL)
		return;
	
	const float2 center(m_frameInfo->center());
	int highestPoint = m_frameInfo->highestPoint();
	
	glColor4f(1.0, 1.0, 1.0, 1.0);
	
	if (m_viewType == Overhead)
	{
		glBegin(GL_POINTS);
			glVertex3f(center[0] - MAXX, center[1] - MAXY, 0);
		glEnd();
	}
	else
	{
		glBegin(GL_LINES);
			glVertex3f(center[0] - MAXX, center[1] - MAXY, 0);
			glVertex3f(center[0] - MAXX, center[1] - MAXY, highestPoint);
		glEnd();
	}
	
	// These match the values in ThighFilter.m
	float rT = 1.0;
	float a = 0.2;
	float b = 0.5;
	float minAbMod = 0.25;
	float extent = 4.0;
	
	float baseRadius = (minAbMod * a + rT) / extent;
	float topRadius = (a + rT) / extent;
	
	glPushMatrix();
		glTranslatef(MINX, MINY, 0.0);
		Convolution::thighTransform(m_frameInfo, false, false);
		glRotatef(-45.0, 1.0, 0.0, 0.0);
		gluCylinder(m_quadric, baseRadius, topRadius, 1.0, 10, 10);
	glPopMatrix();
}
