#define GL_GLEXT_PROTOTYPES 1

#include "dataunit.h"

#include <QGLFramebufferObject>

#ifdef Q_OS_DARWIN
#include <OpenGL/glext.h>
#else
#include <GL/glext.h>
#endif

DataUnit::DataUnit(uint textureId)
	: m_type(Input),
	  m_fbo(NULL),
	  m_textureId(textureId)
{
}

DataUnit::DataUnit(QGLFramebufferObject* fbo)
	: m_type(Input | Output),
	  m_fbo(fbo),
	  m_textureId(fbo->texture())
{
}

DataUnit::DataUnit()
	: m_type(Output),
	  m_fbo(NULL),
	  m_textureId(0)
{
}

DataUnit::~DataUnit()
{
	if (m_fbo != NULL)
		delete m_fbo;
	else if (m_textureId != 0)
		glDeleteTextures(1, &m_textureId);
}

void DataUnit::bindInput(int texUnit)
{
	if (!(m_type & Input))
		qFatal("Tried to bind an output-only DataUnit as input");

	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, m_textureId);
}

void DataUnit::bindOutput()
{
	if (!(m_type & Output))
		qFatal("Tried to bind an input-only DataUnit as output");
	
	if (m_fbo != NULL)
		m_fbo->bind();
}

void DataUnit::releaseInput(int texUnit)
{
}

void DataUnit::releaseOutput()
{
	if (m_fbo != NULL)
		m_fbo->release();
}


