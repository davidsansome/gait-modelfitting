#ifndef GAUSSIAN_H
#define GAUSSIAN_H

#include "filterset.h"

class ShaderPair;

class Gaussian : public FilterSet
{
public:
	Gaussian();
	~Gaussian();

	void init(CGcontext context, CGprofile vertProfile, CGprofile fragProfile, const QSize& imageSize);

	int passCount() { return 2; }
	void bindShaders(int pass);

private:
	ShaderPair* m_xShaders;
	ShaderPair* m_yShaders;
};

#endif
