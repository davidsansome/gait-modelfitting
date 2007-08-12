#include "engine.h"
#include "shaderpair.h"

#include <QImage>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>

Engine::Engine()
	: QGLWidget(),
	  m_texture(0),
	  m_shadersEnabled(false),
	  m_shaders(NULL)
{
	m_redrawTimer = new QTimer(this);
	connect(m_redrawTimer, SIGNAL(timeout()), SLOT(update()));
	m_redrawTimer->setSingleShot(true);
}

Engine::~Engine()
{
	glDeleteTextures(1, &m_texture);
}

void Engine::setImage(const QString& fileName)
{
	glDeleteTextures(1, &m_texture);
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);

	QImage image(fileName);
	m_textureSize = image.size();
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
	setPixelStep();
}

void Engine::loadShaders(const QString& vert, const QString& frag)
{
	delete m_shaders;
	m_shaders = new ShaderPair(m_cgContext);

	try
	{
		m_shaders->loadVertexProgram(vert, m_vertProfile);
		m_shaders->loadFragmentProgram(frag, m_fragProfile);
	}
	catch (QString e)
	{
		QMessageBox::critical(this, "Error loading shaders", e);
		
		delete m_shaders;
		m_shaders = NULL;
		setShadersEnabled(false);
	}

	setPixelStep();
}

void Engine::setShadersEnabled(bool enabled)
{
	if (enabled && m_shaders)
	{
		m_shaders->bind();
		cgGLEnableProfile(m_fragProfile);
		cgGLEnableProfile(m_vertProfile);
	}
	else
	{
		cgGLDisableProfile(m_fragProfile);
		cgGLDisableProfile(m_vertProfile);
	}
}

void errorHandler(CGcontext context, CGerror error, void*)
{
	QMessageBox::warning(NULL, "Cg Error", QString(cgGetErrorString(error)));
}

void Engine::initializeGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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

void Engine::paintGL()
{
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(-1.0f, 1.0f);

		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(1.0f, 1.0f);

		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(1.0f, -1.0f);

		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(-1.0f, -1.0f);
	glEnd();

	m_redrawTimer->start(0);
}

void Engine::setPixelStep()
{
	if (!m_shaders || !m_textureSize.isValid())
		return;

	cgGLSetParameter1f(cgGetNamedParameter(m_shaders->frag(), "pixelStep"), 1.0f / float(m_textureSize.width()));
}

