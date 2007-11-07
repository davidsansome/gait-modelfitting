#ifndef ANALYSIS_H
#define ANALYSIS_H

class Voxel_Space;

class Analysis
{
public:
	Analysis(Voxel_Space* vspace);
	~Analysis();

	int findCenter();

private:
	Voxel_Space* m_vspace;
};

#endif
