#ifndef FILTERSET_H
#define FILTERSET_H

#include <Cg/cg.h>
#include <Cg/cgGL.h>

#include <QSize>

class QGLFramebufferObject;
class DataUnit;

class FilterSet
{
public:
	virtual ~FilterSet() {}

	virtual void init(CGcontext context, CGprofile vertProfile, CGprofile fragProfile, const QSize& imageSize) = 0;
	virtual void setDataUnit(int index, DataUnit* du) = 0;

	virtual int passCount() const = 0;
	virtual void bind(int pass) = 0;
	virtual void release(int pass) = 0;
};

#endif
