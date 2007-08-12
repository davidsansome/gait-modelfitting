#include "engine.h"
#include "filterset.h"

#include <QImage>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>

Engine::Engine()
	: QGLWidget(),
	  m_imageTexture(0),
	  m_filterSet(NULL),
	  m_pbuffer(NULL),
	  m_pbufferTexture(0)
{
	m_redrawTimer = new QTimer(this);
	connect(m_redrawTimer, SIGNAL(timeout()), SLOT(update()));
	m_redrawTimer->setSingleShot(true);
}

Engine::~Engine()
{
	if (m_pbuffer)
		m_pbuffer->releaseFromDynamicTexture();
	glDeleteTextures(1, &m_pbufferTexture);
	glDeleteTextures(1, &m_imageTexture);

	delete m_pbuffer;
	delete m_filterSet;
}

void Engine::setImage(const QString& fileName)
{
	QImage image(fileName);
	m_imageSize = image.size();

	delete m_pbuffer;
	m_pbuffer = new QGLPixelBuffer(m_imageSize, format(), this);
	initPbuffer();
	
	glDeleteTextures(1, &m_imageTexture);
	m_imageTexture = bindTexture(image);
	resize(image.width(), image.height());
}

void Engine::setFilterSet(FilterSet* filterSet)
{
	delete m_filterSet;
	m_filterSet = filterSet;
	m_filterSet->init(m_cgContext, m_vertProfile, m_fragProfile, m_imageSize);
}

void errorHandler(CGcontext context, CGerror error, void*)
{
	QMessageBox::warning(NULL, "Cg Error", QString(cgGetErrorString(error)));
}

void Engine::initializeGL()
{
	initCommon();
	
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
	initMatrices();
}

void Engine::initMatrices()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Engine::initCommon()
{
	glEnable(GL_TEXTURE_2D);
}

void Engine::initPbuffer()
{
	// set up the pbuffer context
	m_pbuffer->makeCurrent();
	initCommon();

	glViewport(0, 0, m_pbuffer->size().width(), m_pbuffer->size().height());
	initMatrices();

	// generate a texture that has the same size/format as the pbuffer
	m_pbufferTexture = m_pbuffer->generateDynamicTexture();

	// bind the dynamic texture to the pbuffer - this is a no-op under X11
	m_pbuffer->bindToDynamicTexture(m_pbufferTexture);
	makeCurrent();
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
	
	m_pbuffer->makeCurrent();
	setShadersEnabled(true);

	uint texture = m_imageTexture;
	for (int i=0 ; i<m_filterSet->passCount() ; ++i)
	{
		m_filterSet->bindShaders(i);
		glBindTexture(GL_TEXTURE_2D, texture);
		drawRect();
		glFlush();

#if  defined(Q_WS_X11)
		// rendering directly to a texture is not supported on X11, unfortunately
		m_pbuffer->updateDynamicTexture(m_pbufferTexture);
#endif
		texture = m_pbufferTexture;
	}

	makeCurrent();
	setShadersEnabled(false);
	glBindTexture(GL_TEXTURE_2D, texture);
	drawRect();

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

