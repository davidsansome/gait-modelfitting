#include "intensitygradient.h"
#include "shaderpair.h"

IntensityGradient::IntensityGradient()
 : FilterSet()
{
}

IntensityGradient::~IntensityGradient()
{
}

void IntensityGradient::init(CGcontext context, CGprofile vertProfile, CGprofile fragProfile, const QSize& imageSize)
{
	m_horizontal = new ShaderPair(context);
	m_horizontal->loadVertexProgram("shaders/intensityGradient/vert.cg", vertProfile);
	m_horizontal->loadFragmentProgram("shaders/intensityGradient/horizfrag.cg", fragProfile);

	m_vertical = new ShaderPair(context);
	m_vertical->loadVertexProgram("shaders/intensityGradient/vert.cg", vertProfile);
	m_vertical->loadFragmentProgram("shaders/intensityGradient/vertfrag.cg", fragProfile);

	cgGLSetParameter2f(cgGetNamedParameter(m_horizontal->frag(), "pixelStep"), 1.0f / float(imageSize.width()), 1.0f / float(imageSize.height()));
	cgGLSetParameter2f(cgGetNamedParameter(m_vertical->frag(),   "pixelStep"), 1.0f / float(imageSize.width()), 1.0f / float(imageSize.height()));
}

void IntensityGradient::bindShaders(int pass)
{
	m_vertical->bind();
}


