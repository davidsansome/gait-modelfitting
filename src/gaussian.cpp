#include "gaussian.h"
#include "shaderpair.h"

Gaussian::Gaussian()
	: FilterSet()
{
}

Gaussian::~Gaussian()
{
	delete m_xShaders;
	delete m_yShaders;
}

void Gaussian::init(CGcontext context, CGprofile vertProfile, CGprofile fragProfile, const QSize& imageSize)
{
	m_xShaders = new ShaderPair(context);
	m_xShaders->loadVertexProgram("shaders/gaussian/vert.cg", vertProfile);
	m_xShaders->loadFragmentProgram("shaders/gaussian/xfrag.cg", fragProfile);

	m_yShaders = new ShaderPair(context);
	m_yShaders->loadVertexProgram("shaders/gaussian/vert.cg", vertProfile);
	m_yShaders->loadFragmentProgram("shaders/gaussian/yfrag.cg", fragProfile);

	cgGLSetParameter1f(cgGetNamedParameter(m_xShaders->frag(), "pixelStep"), 1.0f / float(imageSize.width()));
	cgGLSetParameter1f(cgGetNamedParameter(m_yShaders->frag(), "pixelStep"), 1.0f / float(imageSize.height()));
}

void Gaussian::bindShaders(int pass)
{
	switch (pass)
	{
		case 0: m_xShaders->bind(); break;
		case 1: m_yShaders->bind(); break;
	}
}


