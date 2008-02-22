#ifndef FRAMEINFO_H
#define FRAMEINFO_H

#include "types.h"

#include <QFuture>
#include <QMetaType>

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

enum Limb
{
	LeftThigh,
	RightThigh
};

class Params
{
public:
	Params() : valid(false) {}
	Params(float a, float t);
	Params(const Params& other);
	
	bool valid;
	float alpha;
	float theta;
};

typedef QPair<FrameInfo*, Limb> MapArgs;
typedef QPair<Params, MapArgs> MapType;
typedef QPair<Params, float> ReduceType;

ReduceType correlateMap(const MapType& p);
void correlateReduce(ReduceType& result, const ReduceType& intermediate);

Q_DECLARE_METATYPE(QFuture<void>)

typedef QPair<QString, QFuture<void> > MapReduceOperation;

class FrameInfo : public QObject
{
	Q_OBJECT
	
	friend ReduceType correlateMap(const MapType& p);
	
public:
	static const Model* thighModel() { return s_thighModel; }
	
	FrameInfo(const QString& filename);
	~FrameInfo();
	
	bool hasModelInformation() const { return m_params.valid; }
	QList<MapReduceOperation> update();
	
	Mat4 limbMatrix(Limb limb, const Params& p = Params()) const;
	
	const Voxel_Space* vspace() const { return m_vspace; }
	const Voxel_Space* edgeVspace() const { return m_edgeVspace; }
	const Mesh* mesh() const { return m_mesh; }

	Vec2 center() const { return m_center; }
	int highestPoint() const { return m_highest; }
	int xWidth() const { return m_xWidth; }
	Params params() const { return m_params; }
	
private slots:
	void mapReduceFinished();

private:
	float energy(Limb limb, const Params& params) const;
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
	
	Params m_params;
	
	QFutureWatcher<ReduceType>* m_futureWatcher;
};

#endif
