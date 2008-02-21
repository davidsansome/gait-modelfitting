#ifndef MESHFILTER_H
#define MESHFILTER_H

#include <svl/SVL.h>
#include <QString>
#include <QPair>
#include <vspace.hh>
#include <QFuture>

class MeshFilter;

typedef QPair<Vec, MeshFilter*> MapType;
typedef QPair<Vec, float> ReduceType;

ReduceType correlateMap(const MapType& p);
void correlateReduce(ReduceType& result, const ReduceType& intermediate);

class Model;
class FrameInfo;

class MeshFilter
{
	friend ReduceType correlateMap(const MapType& p);
	
public:
	MeshFilter(const QString& modelName, const QString& lookupTableName);
	~MeshFilter();
	
	Mat4 matrix(const FrameInfo* info, Vec vec) const;
	
	QFuture<ReduceType> correlate(const FrameInfo* info);
	
	const Model* model() const { return m_model; }

private:
	float energy(Vec vec) const;
	float doSearch(const Voxel_Space& voxelSpace, int x, int y, int z) const;
	
	const FrameInfo* m_info;
	const Model* m_model;
	
	quint32 m_lookupElements;
	char* m_lookupData;
	Voxel_Space* m_edgeVspace;
};

#endif
