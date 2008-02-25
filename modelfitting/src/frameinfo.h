#ifndef FRAMEINFO_H
#define FRAMEINFO_H

#include "types.h"

#include <QFuture>
#include <QMetaType>
#include <QMutex>
#include <QSettings>

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

template <typename T>
class Params
{
public:
	Params() : valid(false) {}
	Params(T ta, T tt, T la, T lt);
	Params(const Params& other);
	Params(QSettings& settings);
	
	bool operator <(const Params& other) const;
	Params& operator =(const Params& other);
	
	void save(QSettings& settings) const;
	
	bool valid;
	T thighAlpha;
	T thighTheta;
	T lowerLegAlpha;
	T lowerLegTheta;
};

template <typename T>
QDebug operator <<(QDebug s, const Params<T>& p)
{
	s.nospace() << "Params(" << p.thighAlpha << ", " << p.thighTheta << ", " << p.lowerLegAlpha << ", " << p.lowerLegTheta << ")";
	return s.space();
}

template <typename T>
Params<T>::Params(T ta, T tt, T la, T lt)
	: valid(true),
	  thighAlpha(ta),
	  thighTheta(tt),
	  lowerLegAlpha(la),
	  lowerLegTheta(lt)
{
}

template <typename T>
Params<T>::Params(const Params& other)
{
	*this = other;
}

template <typename T>
Params<T>::Params(QSettings& settings)
	: valid(true)
{
	thighAlpha = settings.value("ThighAlpha", 0.0).value<T>();
	thighTheta = settings.value("ThighTheta", 0.0).value<T>();
	lowerLegAlpha = settings.value("LowerLegAlpha", 0.0).value<T>();
	lowerLegTheta = settings.value("LowerLegTheta", 0.0).value<T>();
}

template <typename T>
Params<T>& Params<T>::operator =(const Params& other)
{
	valid = other.valid;
	thighAlpha = other.thighAlpha;
	thighTheta = other.thighTheta;
	lowerLegAlpha = other.lowerLegAlpha;
	lowerLegTheta = other.lowerLegTheta;
	return *this;
}

template <typename T>
bool Params<T>::operator <(const Params& other) const
{
	if (thighAlpha != other.thighAlpha)
		return thighAlpha < other.thighAlpha;
	if (thighTheta != other.thighTheta)
		return thighTheta < other.thighTheta;
	if (lowerLegAlpha != other.lowerLegAlpha)
		return lowerLegAlpha < other.lowerLegAlpha;
	if (lowerLegTheta != other.lowerLegTheta)
		return lowerLegTheta < other.lowerLegTheta;
	return false;
}

template <typename T>
void Params<T>::save(QSettings& settings) const
{
	settings.setValue("ThighAlpha", thighAlpha);
	settings.setValue("ThighTheta", thighTheta);
	settings.setValue("LowerLegAlpha", lowerLegAlpha);
	settings.setValue("LowerLegTheta", lowerLegTheta);
}


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
	
	FrameInfo(const QString& filename);
	~FrameInfo();
	
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
	
	QMutex m_resultMutex;
	QMap<QPair<Params<int>, Part>, float> m_results;
};

#endif
