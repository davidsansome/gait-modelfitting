#ifndef FRAMEINFO_H
#define FRAMEINFO_H

#include "types.h"

#include <QFuture>

class Voxel_Space;
class Convolution;
class Model;
class FrameInfo;
class Mesh;

#define MAXX 38.0
#define MINX (-MAXX)
#define MAXY 183.0
#define MINY (-MAXY)
#define MAXZ 100.0
#define MINZ (-MAXZ)

#define XEXTENT (MAXX*2)
#define YEXTENT (MAXY*2)
#define ZEXTENT (MAXZ*2)

typedef QPair<Vec, FrameInfo*> MapType;
typedef QPair<Vec, float> ReduceType;

ReduceType correlateMap(const MapType& p);
void correlateReduce(ReduceType& result, const ReduceType& intermediate);

class FrameInfo : public QObject
{
	Q_OBJECT
	
	friend ReduceType correlateMap(const MapType& p);
	
public:
	enum Limb
	{
		LeftThigh,
		RightThigh
	};
	
	static const Model* thighModel() { return s_thighModel; }
	
	FrameInfo(const QString& filename);
	~FrameInfo();
	
	bool hasModelInformation() const { return m_featureVec.Elts() != 0; }
	QFuture<ReduceType> update();
	
	Mat4 limbMatrix(Limb limb, const Vec& vec = Vec()) const;
	
	const Voxel_Space* vspace() const { return m_vspace; }
	const Voxel_Space* edgeVspace() const { return m_edgeVspace; }
	const Mesh* mesh() const { return m_mesh; }

	Vec2 center() const { return m_center; }
	int highestPoint() const { return m_highest; }
	int xWidth() const { return m_xWidth; }
	Vec featureVec() const { return m_featureVec; }
	
private slots:
	void mapReduceFinished();

private:
	float energy(Vec vec) const;
	float doSearch(const Voxel_Space& voxelSpace, int x, int y, int z) const;
	
	Voxel_Space* m_vspace;
	Voxel_Space* m_edgeVspace;
	Mesh* m_mesh;
	
	static const Model* s_thighModel;
	static quint32 s_lookupElements;
	static char* s_lookupData;
	
	int m_highest;
	Vec2 m_center;
	int m_xWidth;
	
	Vec m_featureVec;
	
	QFutureWatcher<ReduceType>* m_futureWatcher;
};

#endif
