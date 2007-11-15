#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "types.h"

class Voxel_Space;

class Analysis
{
public:
	Analysis(Voxel_Space* vspace);
	~Analysis();

	void doStuff();
	
	const float2& center() const { return m_center; }

private:
	Voxel_Space* m_vspace;
	
	int m_highest;
	float2 m_center;
};

#endif
