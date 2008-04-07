#ifndef FRAMEINFO_H
#define FRAMEINFO_H

#include "types.h"
#include "params.h"

#include <QFuture>
#include <QMetaType>
#include <QMutex>
#include <QSettings>

class Voxel_Space;
class Convolution;
class Model;
class FrameInfo;
class FrameSet;
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

#define ALPHA_RANGE (M_PI/32.0)
#define THETA_RANGE (M_PI/4.0)
#define ALPHA_RESOLUTION 5
#define THETA_RESOLUTION 10
#define ALPHA_STEP (ALPHA_RANGE / ALPHA_RESOLUTION)
#define THETA_STEP (THETA_RANGE / THETA_RESOLUTION)

enum Limb
{
	Thigh,
	LowerLeg
};

enum Part
{
	LeftLeg,
	RightLeg
};

class MapType
{
public:
	MapType() {}
	MapType(Params<int> i, Params<float> p, FrameInfo* f, Part pa);
	MapType(const MapType& other);
	
	bool operator <(const MapType& other) const;
	MapType& operator =(const MapType& other);
	
	Params<int> indices;
	Params<float> params;
	FrameInfo* frame;
	Part part;
};

class ReduceType
{
public:
	ReduceType() {}
	ReduceType(Params<int> i, Params<float> p, float e);
	ReduceType(const ReduceType& other);
	
	bool operator <(const ReduceType& other) const;
	ReduceType& operator =(const ReduceType& other);
	
	Params<int> indices;
	Params<float> params;
	float energy;
};

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
	
	FrameInfo(FrameSet* frameSet, const QString& filename, bool loadInfoOnly = false);
	~FrameInfo();
	
	FrameSet* frameSet() const { return m_frameSet; }
	
	bool hasModelInformation() const { return m_leftLegParams.valid && m_rightLegParams.valid; }
	QList<MapReduceOperation> update();
	
	Mat4 limbMatrix(Part part, Limb limb, const Params<float>& p = Params<float>()) const;
	
	void setLeftLeg(const Params<float>& p) { m_leftLegParams = p; }
	void setRightLeg(const Params<float>& p) { m_rightLegParams = p; }
	
	QString filename() const { return m_filename; }
	const Voxel_Space* vspace() const { return m_vspace; }
	const Voxel_Space* edgeVspace() const { return m_edgeVspace; }
	const Mesh* mesh() const { return m_mesh; }

	Vec2 center() const { return m_center; }
	int highestPoint() const { return m_highest; }
	int xWidth() const { return m_xWidth; }
	Params<float> leftLeg() const { return m_leftLegParams; }
	Params<float> rightLeg() const { return m_rightLegParams; }
	
	bool hasResults() const { return m_results.count() != 0; }
	float result(const Params<int>& indices, Part part) { return m_results.value(QPair<Params<int>, Part>(indices, part)); }
	
	void load();
	void load(QSettings& settings);
	void save() const;
	void save(QSettings& settings) const;
	
private slots:
	void leftLegFinished();
	void rightLegFinished();

private:
	void initDistanceCache();
	float energy(Part part, const Params<float>& params) const;
	float modelEnergy(const Model* model, const Mat4& mat) const;
	float doSearch(const Voxel_Space& voxelSpace, int x, int y, int z) const;
	void addResult(const Params<int>& indices, Part part, float energy);
	
	FrameSet* m_frameSet;
	QString m_filename;
	Voxel_Space* m_vspace;
	Voxel_Space* m_edgeVspace;
	Mesh* m_mesh;
	
	static const Model* s_thighModel;
	static quint32 s_lookupElements;
	static char* s_lookupData;
	static char* s_lookupEnd;
	
	int m_highest;
	Vec2 m_center;
	int m_xWidth;
	
	QFutureWatcher<ReduceType>* m_leftLegWatcher;
	QFutureWatcher<ReduceType>* m_rightLegWatcher;
	
	Params<float> m_leftLegParams;
	Params<float> m_rightLegParams;
	
	float* m_distanceCache;
	int m_distanceCacheSize;
	
	QMutex m_resultMutex;
	QMap<QPair<Params<int>, Part>, float> m_results;
};

#endif
