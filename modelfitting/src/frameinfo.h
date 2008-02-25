#ifndef FRAMEINFO_H
#define FRAMEINFO_H

#include "types.h"

#include <QFuture>
#include <QMetaType>
#include <QMutex>

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

#define ALPHA_RANGE (M_PI/8.0)
#define THETA_RANGE (M_PI/4.0)
#define ALPHA_RESOLUTION 10
#define THETA_RESOLUTION 20

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

class Params
{
public:
	Params() : valid(false) {}
	Params(float ta, float tt, float la, float lt);
	Params(const Params& other);
	
	bool operator <(const Params& other) const;
	bool operator ==(const Params& other) const;
	
	bool valid;
	float thighAlpha;
	float thighTheta;
	float lowerLegAlpha;
	float lowerLegTheta;
};

QDebug operator <<(QDebug s, const Params& p);

typedef QPair<FrameInfo*, Part> MapArgs;
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
	
	bool hasModelInformation() const { return m_leftLegParams.valid && m_rightLegParams.valid; }
	QList<MapReduceOperation> update();
	
	Mat4 limbMatrix(Part part, Limb limb, const Params& p = Params()) const;
	
	void setLeftLeg(const Params& p) { m_leftLegParams = p; }
	void setRightLeg(const Params& p) { m_rightLegParams = p; }
	
	QString filename() const { return m_filename; }
	const Voxel_Space* vspace() const { return m_vspace; }
	const Voxel_Space* edgeVspace() const { return m_edgeVspace; }
	const Mesh* mesh() const { return m_mesh; }

	Vec2 center() const { return m_center; }
	int highestPoint() const { return m_highest; }
	int xWidth() const { return m_xWidth; }
	Params leftLeg() const { return m_leftLegParams; }
	Params rightLeg() const { return m_rightLegParams; }
	
	float result(const Params& params, Part part) { return m_results.value(QPair<Params, Part>(params, part)); }
	
private slots:
	void leftLegFinished();
	void rightLegFinished();

private:
	void initDistanceCache();
	float energy(Part part, const Params& params) const;
	float modelEnergy(const Model* model, const Mat4& mat) const;
	float doSearch(const Voxel_Space& voxelSpace, int x, int y, int z) const;
	void addResult(Part part, const ReduceType& result);
	
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
	
	Params m_leftLegParams;
	Params m_rightLegParams;
	
	float* m_distanceCache;
	
	QMutex m_resultMutex;
	QMap<QPair<Params, Part>, float> m_results;
};

#endif
