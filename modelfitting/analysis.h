#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "types.h"

class Voxel_Space;

class Analysis
{
public:
	Analysis(Voxel_Space* vspace);
	~Analysis();

	float2 findCenter();

private:
	Voxel_Space* m_vspace;
};

#endif
