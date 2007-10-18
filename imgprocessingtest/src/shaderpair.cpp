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
}

bool ShaderPair::loadFragmentProgram(const QString& sourceFileName, CGprofile profile)
{
	m_frag = loadShader(sourceFileName, profile);
}

CGprogram ShaderPair::loadShader(const QString& fileName, CGprofile profile)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
		qFatal("Could not open file %s", fileName.toAscii().data());
	
	// Load and null terminate the program source
	char* source = new char[file.size() + 1];
	file.read(source, file.size());
	source[file.size()] = 0;
	
	CGprogram program = cgCreateProgram(m_context, CG_SOURCE, source, profile, NULL, NULL);
	if (program)
		cgGLLoadProgram(program);
	else
		qFatal("Error loading Cg program %s", fileName.toAscii().data());

	delete[] source;
	return program;
}

void ShaderPair::bind()
{
	cgGLBindProgram(m_vert);
	cgGLBindProgram(m_frag);
}


