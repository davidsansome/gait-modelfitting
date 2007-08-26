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
	void setDataUnit(int index, DataUnit* du);

	int passCount() const { return 3; }
	void bind(int pass);
	void release(int pass);

private:
	ShaderPair* m_horizontalShaders;
	ShaderPair* m_verticalShaders;
	ShaderPair* m_intensityShaders;

	DataUnit* m_input;
	DataUnit* m_horizontalOutput;
	DataUnit* m_verticalOutput;
	DataUnit* m_intensityOutput;
};

#endif
