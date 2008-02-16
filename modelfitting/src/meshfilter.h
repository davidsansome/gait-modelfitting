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
	
	Mat4 matrix(const FrameInfo* info, float theta) const;
	
	float correlate(const FrameInfo* info);
	
	const Model* model() const { return m_model; }

private:
	float doSearch(const Voxel_Space& voxelSpace, int x, int y, int z);
	
	const Model* m_model;
	
	quint32 m_lookupElements;
	char* m_lookupData;
};

#endif
