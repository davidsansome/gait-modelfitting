#include "chainfilterset.h"

ChainFilterSet::ChainFilterSet()
	: m_joined(false)
{
}

ChainFilterSet::~ChainFilterSet()
{
}

void ChainFilterSet::addFilterSet(FilterSet* filterSet)
{
	m_filterSets << filterSet;
}

void ChainFilterSet::init(CGcontext context, CGprofile vertProfile, CGprofile fragProfile, const QSize& imageSize)
{
	int pass = 0;
	foreach (FilterSet* filterSet, m_filterSets)
	{
		filterSet->init(context, vertProfile, fragProfile, imageSize);
		for (int i=0 ; i<filterSet->passCount() ; ++i)
			m_passes << QPair<FilterSet*, int>(filterSet, pass);
		pass += filterSet->passCount();
	}
}

void ChainFilterSet::setDataUnit(int index, DataUnit* du)
{
	switch (index)
	{
		case 0: m_input = du; break;
		case 1: m_output = du; joinFilterSets(); break;
	}
}

int ChainFilterSet::passCount() const
{
	return m_passes.count();
}

void ChainFilterSet::bind(int pass)
{
	QPair<FilterSet*, int> filterSet = m_passes[pass];
	filterSet.first->bind(pass - filterSet.second);
}

void ChainFilterSet::release(int pass)
{
	QPair<FilterSet*, int> filterSet = m_passes[pass];
	filterSet.first->release(pass - filterSet.second);
}


