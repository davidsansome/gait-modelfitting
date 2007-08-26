#define GL_GLEXT_PROTOTYPES 1

#include "engine.h"
#include "filterset.h"
#include "dataunit.h"

#include <QImage>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>

Engine::Engine()
	: QGLWidget(),
	  m_filterSet(NULL),
	  m_image(NULL)
{
	m_redrawTimer = new QTimer(this);
	connect(m_redrawTimer, SIGNAL(timeout()), SLOT(update()));
	m_redrawTimer->setSingleShot(true);

	m_framebuffer = new DataUnit();
}

Engine::~Engine()
{
	delete m_filterSet;
	delete m_image;
	delete m_framebuffer;
}

void Engine::setImage(const QString& fileName)
{
	QImage image(fileName);
	m_imageSize = image.size();

	delete m_image;
	m_image = new DataUnit(bindTexture(image));
	
	resize(image.width(), image.height());
}

void Engine::setFilterSet(FilterSet* filterSet)
{
	delete m_filterSet;
	m_filterSet = filterSet;
	m_filterSet->init(m_cgContext, m_vertProfile, m_fragProfile, m_imageSize);
	m_filterSet->setDataUnit(0, m_image);
	m_filterSet->setDataUnit(1, m_framebuffer);
}

void errorHandler(CGcontext context, CGerror error, void*)
{
	qFatal("Cg Error: %s", cgGetErrorString(error));
	//QMessageBox::warning(NULL, "Cg Error", QString(cgGetErrorString(error)));
}

void Engine::initializeGL()
{
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	
	// Initialise cg
	cgSetErrorHandler(errorHandler, NULL);
	m_cgContext = cgCreateContext();

	// Find the best profiles and load them
	m_vertProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
	cgGLSetOptimalOptions(m_vertProfile);
	
	m_fragProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
	cgGLSetOptimalOptions(m_fragProfile);
	if (m_vertProfile == CG_PROFILE_UNKNOWN || m_fragProfile == CG_PROFILE_UNKNOWN)
	{
		QMessageBox::critical(NULL, "Shaders not supported", "No suitable shader extensions are supported by your graphics card");
		QCoreApplication::exit(1);
	}
	qDebug() << "Cg profiles:" << cgGetProfileString(m_vertProfile) << cgGetProfileString(m_fragProfile);
}

void Engine::resizeGL(int w, int h)
{
	glViewport(0, 0, width(), height());
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Engine::setShadersEnabled(bool enabled)
{
	if (enabled)
	{
		cgGLEnableProfile(m_fragProfile);
		cgGLEnableProfile(m_vertProfile);
	}
	else
	{
		cgGLDisableProfile(m_fragProfile);
		cgGLDisableProfile(m_vertProfile);
	}
}

void Engine::paintGL()
{
	if (!m_filterSet)
		return;

	setShadersEnabled(true);

	for (int i=0 ; i<m_filterSet->passCount() ; ++i)
	{
		m_filterSet->bind(i);
		drawRect();
		glFlush();
		m_filterSet->release(i);
	}

	m_redrawTimer->start(0);
}

void Engine::drawRect()
{
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(-1.0f, 1.0f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(1.0f, 1.0f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(1.0f, -1.0f);

		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(-1.0f, -1.0f);
	glEnd();
}

