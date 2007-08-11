#ifndef SHADERPAIR_H
#define SHADERPAIR_H

#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include <QString>

class ShaderPair
{
public:
	ShaderPair(CGcontext context);
	~ShaderPair();

	bool loadVertexProgram(const QString& sourceFileName, CGprofile profile);
	bool loadFragmentProgram(const QString& sourceFileName, CGprofile profile);

	void bind();

private:
	CGprogram loadShader(const QString& sourceFileName, CGprofile profile);
	
	CGcontext m_context;
	CGprogram m_vert;
	CGprogram m_frag;
};

#endif
