#ifndef DATAUNIT_H
#define DATAUNIT_H

class QGLFramebufferObject;

#include <QtGlobal>

#ifdef Q_OS_DARWIN
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

class DataUnit
{
public:
	enum Type
	{
		Input = 0x00000001,
		Output = 0x00000002
	};
	
	DataUnit(uint textureId); // Input only
	DataUnit(QGLFramebufferObject* fbo); // Input/output
	DataUnit(); // Output only
	~DataUnit();

	int type() { return m_type; }
	int isInput() { return m_type & Input; }
	int isOutput() { return m_type & Output; }
	
	void bindInput(int texUnit = 0);
	void bindOutput();

	void releaseInput(int texUnit = 0);
	void releaseOutput();

private:
	GLuint m_textureId;
	QGLFramebufferObject* m_fbo;
	int m_type;
};

#endif
