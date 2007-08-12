#ifndef FILTERSET_H
#define FILTERSET_H

#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include <QSize>

class FilterSet
{
public:
	virtual ~FilterSet() {}

	virtual void init(CGcontext context, CGprofile vertProfile, CGprofile fragProfile, const QSize& imageSize) = 0;

	virtual int passCount() = 0;
	virtual void bindShaders(int pass) = 0;
};

#endif
