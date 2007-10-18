#include "gaussian.h"
#include "shaderpair.h"
#include "dataunit.h"

#include <QGLFramebufferObject>

Gaussian::Gaussian()
	: FilterSet()
{
}

Gaussian::~Gaussian()
{
	delete m_xShaders;
	delete m_yShaders;

	if (m_temp != m_output)
		delete m_temp;
}

void Gaussian::init(CGcontext context, CGprofile vertProfile, CGprofile fragProfile, const QSize& imageSize)
{
	m_xShaders = new ShaderPair(context);
	m_xShaders->loadVertexProgram("shaders/gaussian/vert.cg", vertProfile);
	m_xShaders->loadFragmentProgram("shaders/gaussian/xfrag.cg", fragProfile);

	m_yShaders = new ShaderPair(context);
	m_yShaders->loadVertexProgram("shaders/gaussian/vert.cg", vertProfile);
	m_yShaders->loadFragmentProgram("shaders/gaussian/yfrag.cg", fragProfile);

	m_imageSize = imageSize;
	cgGLSetParameter1f(cgGetNamedParameter(m_xShaders->frag(), "pixelStep"), 1.0f / float(imageSize.width()));
	cgGLSetParameter1f(cgGetNamedParameter(m_yShaders->frag(), "pixelStep"), 1.0f / float(imageSize.height()));
}

void Gaussian::setDataUnit(int index, DataUnit* du)
{
	switch (index)
	{
		case 0: m_input = du; break;
		case 1:
			m_output = du;
			if (m_output->isInput())
				m_temp = m_output;
			else
				m_temp = new DataUnit(new QGLFramebufferObject(m_imageSize));
			break;
	}
}

void Gaussian::bind(int pass)
{
	switch (pass)
	{
		case 0:
			m_input->bindInput();
			m_temp->bindOutput();
			m_xShaders->bind();
			break;
		
		case 1:
			m_temp->bindInput();
			m_output->bindOutput();
			m_yShaders->bind();
			break;
	}
}

void Gaussian::release(int pass)
{
	switch (pass)
	{
		case 0:
			m_input->releaseInput();
			m_temp->releaseOutput();
			break;
		case 1:
			m_temp->releaseInput();
			m_output->releaseOutput();
			break;
	}
}


