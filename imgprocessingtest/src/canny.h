#ifndef CANNY_H
#define CANNY_H

#include "chainfilterset.h"

class DataUnit;
class Gaussian;
class IntensityGradient;

class Canny : public ChainFilterSet
{
public:
	Canny();
	~Canny();

	void init(CGcontext context, CGprofile vertProfile, CGprofile fragProfile, const QSize& imageSize);

private:
	void joinFilterSets();

	DataUnit* m_temp;

	Gaussian* m_gaussian;
	IntensityGradient* m_intensityGradient;
};

#endif
