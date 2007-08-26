#ifndef GAUSSIAN_H
#define GAUSSIAN_H

#include "filterset.h"

class ShaderPair;

/*
DataUnit 0 - Input image
DataUnit 1 - Output image
*/
class Gaussian : public FilterSet
{
public:
	Gaussian();
	~Gaussian();

	void init(CGcontext context, CGprofile vertProfile, CGprofile fragProfile, const QSize& imageSize);
	void setDataUnit(int index, DataUnit* du);

	int passCount() const { return 2; }
	void bind(int pass);
	void release(int pass);

private:
	ShaderPair* m_xShaders;
	ShaderPair* m_yShaders;

	DataUnit* m_input;
	DataUnit* m_temp;
	DataUnit* m_output;

	QSize m_imageSize;
};

#endif
