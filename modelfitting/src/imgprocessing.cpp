#define GL_GLEXT_PROTOTYPES 1

#include "imgprocessing.h"
#include "filterset.h"
#include "dataunit.h"
#include "glview.h"

#include <QImage>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QGLContext>

ImgProcessing::ImgProcessing(QWidget* parent)
	: QGLWidget(parent, GLView::s_contextWidget),
	  m_filterSet(NULL)
{
	m_framebuffer = new DataUnit();
	
	if (GLView::s_contextWidget == NULL)
		GLView::s_contextWidget = this;
}

ImgProcessing::~ImgProcessing()
{
	delete m_filterSet;
	delete m_framebuffer;
}

void ImgProcessing::setFilterSet(FilterSet* filterSet)
{
	delete m_filterSet;
	m_filterSet = filterSet;
	
	if (filterSet != NULL)
	{
		m_filterSet->init(m_cgContext, m_vertProfile, m_fragProfile, size());
		m_filterSet->setDataUnit(1, m_framebuffer);
	}
}

void errorHandler(CGcontext context, CGerror error, void*)
{
	qFatal("Cg Error: %s", cgGetErrorString(error));
	//QMessageBox::warning(NULL, "Cg Error", QString(cgGetErrorString(error)));
}

void ImgProcessing::initializeGL()
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
	
	emit setupReady();
}

void ImgProcessing::resizeGL(int w, int h)
{
	glViewport(0, 0, width(), height());
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ImgProcessing::setShadersEnabled(bool enabled)
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

void ImgProcessing::paintGL()
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
	qDebug() << "Drew frame";
	
	setShadersEnabled(false);
}

void ImgProcessing::drawRect()
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
