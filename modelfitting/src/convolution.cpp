#include "convolution.h"
#include "shaderpair.h"
#include "dataunit.h"
#include "filter.h"

#include <QGLFramebufferObject>

Convolution::Convolution(Filter* filter)
	: FilterSet(),
	  m_filter(filter)
{
}

Convolution::~Convolution()
{
	delete m_shaders;
}

void Convolution::init(CGcontext context, CGprofile vertProfile, CGprofile fragProfile, const QSize& imageSize)
{
	m_shaders = new ShaderPair(context);
	m_shaders->loadVertexProgram("shaders/vertexBasic.cg", vertProfile);
	m_shaders->loadFragmentProgram("shaders/convolution.cg", fragProfile);

	m_imageSize = imageSize;
	cgGLSetParameter1f(cgGetNamedParameter(m_shaders->frag(), "pixelStep"), 1.0f / float(imageSize.width()));
}

void Convolution::setDataUnit(int index, DataUnit* du)
{
	switch (index)
	{
		case 1: m_output = du; break;
	}
}

void Convolution::bind(int pass)
{
	switch (pass)
	{
		case 0:
			m_filter->bind();
			m_output->bindOutput();
			m_shaders->bind();
			break;
	}
}

void Convolution::release(int pass)
{
	switch (pass)
	{
		case 0:
			m_output->releaseOutput();
			break;
	}
}


