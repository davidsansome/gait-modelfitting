#ifndef FRAMEINFO_H
#define FRAMEINFO_H

#include "types.h"
#include "params.h"

#include <QFuture>
#include <QMetaType>
#include <QMutex>
#include <QSettings>
#include <QModelIndex>

class Voxel_Space;
class Convolution;
class Model;
class FrameInfo;
class FrameModel;
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
#define THETA_RESOLUTION 40
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


Q_DECLARE_METATYPE(QFuture<void>)

typedef QPair<QString, QFuture<void> > MapReduceOperation;

class FrameInfo : public QObject
{
	Q_OBJECT
	
	friend class FrameModel;
	
public:
	static const Model* thighModel() { return s_thighModel; }
	
	~FrameInfo();
	
	const FrameModel* frameModel() const { return m_frameModel; }
	QModelIndex index() const { return m_index; }
	
	bool hasModelInformation() const { return m_leftLegParams.valid && m_rightLegParams.valid; }
	QList<MapReduceOperation> update();
	
	Mat4 limbMatrix(Part part, Limb limb, const Params<float>& p = Params<float>()) const;
	
	void setLeftLeg(const Params<float>& p) { m_leftLegParams = p; }
	void setRightLeg(const Params<float>& p) { m_rightLegParams = p; }
	
	QString fileName() const { return m_fileName; }
	const Voxel_Space* vspace() const { return m_vspace; }
	const Voxel_Space* edgeVspace() const { return m_edgeVspace; }
	const Mesh* mesh() const { return m_mesh; }

	Vec2 center() const { return m_center; }
	int highestPoint() const { return m_highest; }
	int xWidth() const { return m_xWidth; }
	Params<float> leftLeg() const { return m_leftLegParams; }
	Params<float> rightLeg() const { return m_rightLegParams; }
	
	void load();
	void load(QSettings& settings);
	void save() const;
	void save(QSettings& settings) const;
	
private slots:
	void leftLegFinished();
	void rightLegFinished();

private:
	FrameInfo(const FrameModel* frameModel, const QModelIndex& index, bool loadInfoOnly = false);
	
	Params<float> multiResolutionSearch(Part part);
	Params<float> subSearch(Part part, const Params<float>& min, const Params<float>& max, int alphaResolution, int thetaResolution);
	
	void initDistanceCache();
	float energy(Part part, const Params<float>& params) const;
	float modelEnergy(const Model* model, const Mat4& mat) const;
	float doSearch(const Voxel_Space& voxelSpace, int x, int y, int z) const;
	
	const FrameModel* m_frameModel;
	QModelIndex m_index;
	QString m_fileName;
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
	
	QFutureWatcher<Params<float> >* m_leftLegWatcher;
	QFutureWatcher<Params<float> >* m_rightLegWatcher;
	
	Params<float> m_leftLegParams;
	Params<float> m_rightLegParams;
	
	float* m_distanceCache;
	int m_distanceCacheSize;
	
	QMutex m_resultMutex;
};

#endif
