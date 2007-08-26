#ifndef CHAINFILTERSET_H
#define CHAINFILTERSET_H

#include "filterset.h"

#include <QList>
#include <QPair>

class ChainFilterSet : public FilterSet
{
public:
	ChainFilterSet();
	virtual ~ChainFilterSet();

	virtual void init(CGcontext context, CGprofile vertProfile, CGprofile fragProfile, const QSize& imageSize);
	virtual void setDataUnit(int index, DataUnit* du);

	virtual int passCount() const;
	virtual void bind(int pass);
	virtual void release(int pass);

protected:
	void addFilterSet(FilterSet* filterSet);
	virtual void joinFilterSets() = 0;

	DataUnit* m_input;
	DataUnit* m_output;
	int m_passCount;
	bool m_joined;

	QList<FilterSet*> m_filterSets;
	QList<QPair<FilterSet*, int> > m_passes;
};

#endif
