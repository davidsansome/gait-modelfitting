#ifndef DATAUNIT_H
#define DATAUNIT_H

class QGLFramebufferObject;

typedef unsigned int uint;

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
	
	void bindInput();
	void bindOutput();

	void releaseInput();
	void releaseOutput();

private:
	uint m_textureId;
	QGLFramebufferObject* m_fbo;
	int m_type;
};

#endif
