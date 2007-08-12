#ifndef INTENSITYGRADIENT_H
#define INTENSITYGRADIENT_H

#include "filterset.h"

class ShaderPair;

class IntensityGradient : public FilterSet
{
public:
	IntensityGradient();
	~IntensityGradient();

	void init(CGcontext context, CGprofile vertProfile, CGprofile fragProfile, const QSize& imageSize);

	int passCount() { return 1; }
	void bindShaders(int pass);

private:
	ShaderPair* m_horizontal;
	ShaderPair* m_vertical;
};

#endif
