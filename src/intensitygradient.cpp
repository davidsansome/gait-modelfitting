#include "intensitygradient.h"
#include "shaderpair.h"
#include "dataunit.h"

#include <QGLFramebufferObject>

IntensityGradient::IntensityGradient()
 : FilterSet()
{
}

IntensityGradient::~IntensityGradient()
{
	delete m_horizontalOutput;
	delete m_verticalOutput;
}

void IntensityGradient::init(CGcontext context, CGprofile vertProfile, CGprofile fragProfile, const QSize& imageSize)
{
	m_horizontalShaders = new ShaderPair(context);
	m_horizontalShaders->loadVertexProgram("shaders/intensityGradient/vert.cg", vertProfile);
	m_horizontalShaders->loadFragmentProgram("shaders/intensityGradient/horizfrag.cg", fragProfile);

	m_verticalShaders = new ShaderPair(context);
	m_verticalShaders->loadVertexProgram("shaders/intensityGradient/vert.cg", vertProfile);
	m_verticalShaders->loadFragmentProgram("shaders/intensityGradient/vertfrag.cg", fragProfile);

	m_intensityShaders = new ShaderPair(context);
	m_intensityShaders->loadVertexProgram("shaders/intensityGradient/vert.cg", vertProfile);
	m_intensityShaders->loadFragmentProgram("shaders/intensityGradient/intensityfrag.cg", fragProfile);

	cgGLSetParameter2f(cgGetNamedParameter(m_horizontalShaders->frag(), "pixelStep"), 1.0f / float(imageSize.width()), 1.0f / float(imageSize.height()));
	cgGLSetParameter2f(cgGetNamedParameter(m_verticalShaders->frag(),   "pixelStep"), 1.0f / float(imageSize.width()), 1.0f / float(imageSize.height()));

	m_horizontalOutput = new DataUnit(new QGLFramebufferObject(imageSize));
	m_verticalOutput = new DataUnit(new QGLFramebufferObject(imageSize));
}

void IntensityGradient::setDataUnit(int index, DataUnit* du)
{
	switch (index)
	{
		case 0: m_input = du; break;
		case 1: m_intensityOutput = du; break;
	}
}

void IntensityGradient::bind(int pass)
{
	switch (pass)
	{
		case 0:
			m_input->bindInput();
			m_horizontalOutput->bindOutput();
			m_horizontalShaders->bind();
			break;
		case 1:
			m_input->bindInput();
			m_verticalOutput->bindOutput();
			m_verticalShaders->bind();
			break;
		case 2:
			m_verticalOutput->bindInput(0);
			m_horizontalOutput->bindInput(1);
			m_intensityOutput->bindOutput();
			m_intensityShaders->bind();
			break;
	}
}

void IntensityGradient::release(int pass)
{
	switch (pass)
	{
		case 0:
			m_input->releaseInput();
			m_horizontalOutput->releaseOutput();
			break;
		case 1:
			m_input->releaseInput();
			m_verticalOutput->releaseOutput();
			break;
		case 2:
			m_verticalOutput->releaseInput(0);
			m_horizontalOutput->releaseInput(1);
			m_intensityOutput->releaseOutput();
			break;
	}
}


