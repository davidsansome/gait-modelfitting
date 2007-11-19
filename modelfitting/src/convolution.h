#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include "filterset.h"

class Filter;
class ShaderPair;

class Convolution : public FilterSet
{
public:
	Convolution(Filter* filter);
	~Convolution();

	void init(CGcontext context, CGprofile vertProfile, CGprofile fragProfile, const QSize& imageSize);
	void setDataUnit(int index, DataUnit* du);

	int passCount() const { return 1; }
	void bind(int pass);
	void release(int pass);

private:
	ShaderPair* m_shaders;

	DataUnit* m_output;
	Filter* m_filter;

	QSize m_imageSize;
};

#endif