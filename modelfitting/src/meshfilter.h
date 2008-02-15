#ifndef MESHFILTER_H
#define MESHFILTER_H

class MeshFilter
{
public:
	MeshFilter(const QString& fileName);
	
	void correlate(Voxel_Space* vspace);
};

#endif
