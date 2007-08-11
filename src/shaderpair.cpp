#include "shaderpair.h"

#include <QFile>

ShaderPair::ShaderPair(CGcontext context)
	: m_context(context)
{
}

ShaderPair::~ShaderPair()
{
	if (cgIsProgram(m_vert))
		cgDestroyProgram(m_vert);
	if (cgIsProgram(m_frag))
		cgDestroyProgram(m_frag);
}

bool ShaderPair::loadVertexProgram(const QString& sourceFileName, CGprofile profile)
{
	m_vert = loadShader(sourceFileName, profile);
	return cgIsProgram(m_vert);
}

bool ShaderPair::loadFragmentProgram(const QString& sourceFileName, CGprofile profile)
{
	m_frag = loadShader(sourceFileName, profile);
	return cgIsProgram(m_frag);
}

CGprogram ShaderPair::loadShader(const QString& fileName, CGprofile profile)
{
	QFile file(fileName);
	file.open(QIODevice::ReadOnly);
	
	// Load and null terminate the program source
	char* source = new char[file.size() + 1];
	file.read(source, file.size());
	source[file.size()] = 0;
	
	CGprogram program = cgCreateProgram(m_context, CG_SOURCE, source, profile, NULL, NULL);
	if (program)
		cgGLLoadProgram(program);

	delete[] source;
	return program;
}

void ShaderPair::bind()
{
	cgGLBindProgram(m_vert);
	cgGLBindProgram(m_frag);
}


