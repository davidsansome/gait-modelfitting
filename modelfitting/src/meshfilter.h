#ifndef MESHFILTER_H
#define MESHFILTER_H

#include <svl/SVL.h>
#include <QString>
#include <vspace.hh>

class Model;
class FrameInfo;

class MeshFilter
{
public:
	MeshFilter(const QString& modelName, const QString& lookupTableName);
	~MeshFilter();
	
	void correlate(FrameInfo* info);
	void draw(FrameInfo* info, float theta);

private:
	float doSearch(const Voxel_Space& voxelSpace, int x, int y, int z);
	
	const Model* m_model;
	
	quint32 m_lookupElements;
	char* m_lookupData;
};

#endif
