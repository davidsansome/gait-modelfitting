#include "frameinfo.h"
#include "model.h"
#include "modelloader.h"
#include "listmodels.h"

#include <QDebug>
#include <QTime>
#include <QFutureWatcher>
#include <QFile>
#include <QtConcurrentRun>
#include <QtConcurrentMap>
#include <QMutexLocker>
#include <QPointF>
#include <QApplication>

#include <vspace.hh>
#include <mesh.hh>

const Model* FrameInfo::s_thighModel = NULL;
quint32 FrameInfo::s_lookupElements = 0;
char* FrameInfo::s_lookupData = NULL;
char* FrameInfo::s_lookupEnd = NULL;


#ifndef USE_MULTIRESOLUTION

MapType::MapType(Params<int> i, Params<float> p, FrameInfo* f, Part pa)
	: indices(i),
	  params(p),
	  frame(f),
	  part(pa)
{
}

MapType::MapType(const MapType& other)
{
	*this = other;
}

MapType& MapType::operator =(const MapType& other)
{
	indices = other.indices;
	params = other.params;
	frame = other.frame;
	part = other.part;
	return *this;
}

bool MapType::operator <(const MapType& other) const
{
	return params < other.params;
}

ReduceType::ReduceType(Params<int> i, Params<float> p, float e)
	: indices(i),
	  params(p),
	  energy(e)
{
}

ReduceType::ReduceType(const ReduceType& other)
{
	*this = other;
}

ReduceType& ReduceType::operator =(const ReduceType& other)
{
	indices = other.indices;
	params = other.params;
	energy = other.energy;
}

bool ReduceType::operator <(const ReduceType& other) const
{
	return params < other.params;
}

#endif


FrameInfo::FrameInfo(const FrameModel* frameModel, const QModelIndex& index, bool loadInfoOnly)
	: m_distanceCache(NULL),
	  m_index(index),
	  m_fileName(frameModel->filePath(index)),
	  m_distanceCacheSize(-1),
	  m_vspace(NULL),
	  m_mesh(NULL),
	  m_edgeVspace(NULL),
	  m_leftLegWatcher(NULL),
	  m_rightLegWatcher(NULL)
{
	if (loadInfoOnly)
	{
		load();
		return;
	}
	
	m_vspace = new Voxel_Space(m_fileName.toAscii().data());
	m_mesh = new Mesh(*m_vspace);
	if (QApplication::type() != QApplication::Tty)
		m_mesh->draw_init(true);
	
	m_edgeVspace = new Voxel_Space(*m_vspace);
	m_edgeVspace->edge_detect();
	
#ifndef USE_MULTIRESOLUTION
	m_leftLegWatcher = new QFutureWatcher<ReduceType>(this);
	m_rightLegWatcher = new QFutureWatcher<ReduceType>(this);
#else
	m_leftLegWatcher = new QFutureWatcher<Params<float> >(this);
	m_rightLegWatcher = new QFutureWatcher<Params<float> >(this);
#endif
	connect(m_leftLegWatcher, SIGNAL(finished()), SLOT(leftLegFinished()));
	connect(m_rightLegWatcher, SIGNAL(finished()), SLOT(rightLegFinished()));
	
	load();
	
	if (s_thighModel == NULL)
	{
		// Load model
		s_thighModel = ModelLoader::instance()->model("thighmodel");
		
		// Load lookup table
		QFile lookupTable(":meshsearch.lut");
		lookupTable.open(QIODevice::ReadOnly);
		QDataStream s(&lookupTable);
		
		s >> s_lookupElements;
		s_lookupData = new char[s_lookupElements * 3];
		s.readRawData(s_lookupData, s_lookupElements * 3);
		s_lookupEnd = s_lookupData + s_lookupElements;
	}
}

FrameInfo::~FrameInfo()
{
	if (m_mesh && QApplication::type() != QApplication::Tty)
		m_mesh->draw_destroy();
	
	delete m_mesh;
	delete m_vspace;
	delete m_edgeVspace;
	delete[] m_distanceCache;
}

void FrameInfo::leftLegFinished()
{
	if (m_leftLegWatcher->isCanceled())
		return;
	
#ifndef USE_MULTIRESOLUTION
	m_leftLegParams = m_leftLegWatcher->future().result().params;
#else
	m_leftLegParams = m_leftLegWatcher->future().result();
#endif

	//qDebug() << "Final params for left leg =" << m_leftLegParams;
	
	if (hasModelInformation())
		save();
}

void FrameInfo::rightLegFinished()
{
	if (m_rightLegWatcher->isCanceled())
		return;
	
#ifndef USE_MULTIRESOLUTION
	m_rightLegParams = m_rightLegWatcher->future().result().params;
#else
	m_rightLegParams = m_rightLegWatcher->future().result();
#endif
	//qDebug() << "Final params for right leg =" << m_rightLegParams;
	
	if (hasModelInformation())
		save();
}

QList<MapReduceOperation> FrameInfo::update()
{
	unsigned long long xTotal = 0;
	unsigned long long yTotal = 0;
	unsigned int i = 0;
	
	int highest = 0;
	int minX = m_vspace->x_size;
	int maxX = 0;
	
	for (int x=0 ; x<m_vspace->x_size ; ++x)
	{
		for (int y=0 ; y<m_vspace->y_size ; ++y)
		{
			for (int z=0 ; z<m_vspace->z_size ; ++z)
			{
				if (!m_vspace->get(x, y, z))
					continue;
				
				xTotal += x;
				yTotal += y;
				i++;
				
				if (z > highest) highest = z;
				if (x > maxX) maxX = x;
				if (x < minX) minX = x;
			}
		}
	}
	
	m_highest = highest;
	m_center = Vec2(float(xTotal)/i, float(yTotal)/i);
	m_xWidth = maxX - minX;
	
	if (m_distanceCache == NULL)
		initDistanceCache();
	
#ifndef USE_MULTIRESOLUTION
	m_results.clear();
#endif
	m_leftLegParams.invalidate();
	m_rightLegParams.invalidate();
	
	QList<MapReduceOperation> ret;
	
#ifndef USE_MULTIRESOLUTION
	// Construct list of potential parameters
	QList<MapType> params;
	for (int ta=-ALPHA_RESOLUTION ; ta<=ALPHA_RESOLUTION ; ta++)
		for (int tt=-THETA_RESOLUTION ; tt<=THETA_RESOLUTION ; tt++)
			for (int la=-ALPHA_RESOLUTION ; la<=ALPHA_RESOLUTION ; la++)
				for (int lt=-THETA_RESOLUTION ; lt<=THETA_RESOLUTION ; lt++)
					params << MapType(Params<int>(ta, tt, la, lt),
					                  Params<float>(ta*ALPHA_STEP, tt*THETA_STEP, la*ALPHA_STEP, lt*THETA_STEP),
					                  this, LeftLeg);
	
	// Start the mapreduce
	QFuture<ReduceType> leftLegFuture = QtConcurrent::mappedReduced(params, correlateMap, correlateReduce);
	
	params.clear();
	for (int ta=-ALPHA_RESOLUTION ; ta<=ALPHA_RESOLUTION ; ta++)
		for (int tt=-THETA_RESOLUTION ; tt<=THETA_RESOLUTION ; tt++)
			for (int la=-ALPHA_RESOLUTION ; la<=ALPHA_RESOLUTION ; la++)
				for (int lt=-THETA_RESOLUTION ; lt<=THETA_RESOLUTION ; lt++)
					params << MapType(Params<int>(ta, tt, la, lt),
					                  Params<float>(ta*ALPHA_STEP, tt*THETA_STEP, la*ALPHA_STEP, lt*THETA_STEP),
					                  this, RightLeg);
	
	// Start the mapreduce
	QFuture<ReduceType> rightLegFuture = QtConcurrent::mappedReduced(params, correlateMap, correlateReduce);
#else
	QFuture<Params<float> > leftLegFuture = QtConcurrent::run(this, &FrameInfo::multiResolutionSearch, LeftLeg);
	QFuture<Params<float> > rightLegFuture = QtConcurrent::run(this, &FrameInfo::multiResolutionSearch, RightLeg);
#endif
	
	m_leftLegWatcher->setFuture(leftLegFuture);
	ret << MapReduceOperation("Left leg", leftLegFuture);
	m_rightLegWatcher->setFuture(rightLegFuture);
	ret << MapReduceOperation("Right leg", rightLegFuture);
	
	return ret;
}

#ifdef USE_MULTIRESOLUTION
Params<float> FrameInfo::multiResolutionSearch(Part part)
{
	const int firstAlphaResolution = 7;
	const int firstThetaResolution = 11;
	const float alphaError = (2.0 * ALPHA_RANGE) / (firstAlphaResolution - 1);
	const float thetaError = (2.0 * THETA_RANGE) / (firstThetaResolution - 1);
	const int secondAlphaResolution = 6;
	const int secondThetaResolution = 8;
	
	// Low resolution search
	Params<float> min(-ALPHA_RANGE, -THETA_RANGE, -ALPHA_RANGE, -THETA_RANGE);
	Params<float> max(+ALPHA_RANGE, +THETA_RANGE, +ALPHA_RANGE, +THETA_RANGE);
	
	Params<float> res(subSearch(part, min, max, firstAlphaResolution, firstThetaResolution));
	
	// High resolution search around the result
	min = Params<float>(res.thighAlpha - alphaError,
	                    res.thighTheta - thetaError,
	                    res.lowerLegAlpha - alphaError,
	                    res.lowerLegTheta - thetaError);
	max = Params<float>(res.thighAlpha + alphaError,
	                    res.thighTheta + thetaError,
	                    res.lowerLegAlpha + alphaError,
	                    res.lowerLegTheta + thetaError);
	
	res = subSearch(part, min, max, secondAlphaResolution, secondThetaResolution);
	
	return res;
}

Params<float> FrameInfo::subSearch(Part part, const Params<float>& min, const Params<float>& max, int alphaResolution, int thetaResolution)
{
	Params<float> step = Params<float>(
		(max.thighAlpha - min.thighAlpha) / alphaResolution,
		(max.thighTheta - min.thighTheta) / thetaResolution,
		(max.lowerLegAlpha - min.lowerLegAlpha) / alphaResolution,
		(max.lowerLegTheta - min.lowerLegTheta) / thetaResolution);
	
	Params<float> retParams;
	float retEnergy = std::numeric_limits<float>::max();
	
	Params<int> i;
	for (i.thighAlpha=0 ; i.thighAlpha<alphaResolution ; ++i.thighAlpha)
		for (i.thighTheta=0 ; i.thighTheta<thetaResolution ; ++i.thighTheta)
			for (i.lowerLegAlpha=0 ; i.lowerLegAlpha<alphaResolution ; ++i.lowerLegAlpha)
				for (i.lowerLegTheta=0 ; i.lowerLegTheta<thetaResolution; ++i.lowerLegTheta)
				{
					Params<float> p(
						min.thighAlpha + step.thighAlpha * i.thighAlpha,
						min.thighTheta + step.thighTheta * i.thighTheta,
						min.lowerLegAlpha + step.lowerLegAlpha * i.lowerLegAlpha,
						min.lowerLegTheta + step.lowerLegTheta * i.lowerLegTheta);
					
					float e = energy(part, p);
					if (e < retEnergy)
					{
						retEnergy = e;
						retParams = p;
					}
				}
	
	return retParams;
}

#else

ReduceType correlateMap(const MapType& p)
{
	float energy = p.frame->energy(p.part, p.params);
	
	ReduceType ret(p.indices, p.params, energy);
	p.frame->addResult(p.indices, p.part, energy);
	
	return ret;
}

void correlateReduce(ReduceType& result, const ReduceType& intermediate)
{
	if (!result.params.valid || intermediate.energy < result.energy)
		result = intermediate;
}

void FrameInfo::addResult(const Params<int>& indices, Part part, float energy)
{
	QMutexLocker locker(&m_resultMutex);
	m_results[QPair<Params<int>, Part>(indices, part)] = energy;
}
#endif

float FrameInfo::energy(Part part, const Params<float>& params) const
{
	float ret = 0.0;
	ret += modelEnergy(s_thighModel, limbMatrix(part, Thigh, params));
	ret += modelEnergy(s_thighModel, limbMatrix(part, LowerLeg, params));
	
	return ret;
}

float FrameInfo::modelEnergy(const Model* model, const Mat4& mat) const
{
	float ret = 0.0;
	
	const Vertex* vertex = model->vertexData();
	for (int i=0 ; i<model->vertexCount() ; ++i)
	{
		const Vec3 pos(proj(mat * vertex->pos));
		int x = int(pos[0]); // These get floored, that's ok
		int y = int(pos[1]);
		int z = int(pos[2]);
		
		if (vertex->mat == 0) // Search for nearby edge voxels
			ret += doSearch(*m_edgeVspace, x, y, z);
		else // Search for nearby filled voxels
			ret += doSearch(*m_vspace, x, y, z);
		
		vertex++;
	}
	
	return ret;
}

float FrameInfo::doSearch(const Voxel_Space& voxelSpace, int x, int y, int z) const
{
	// See if this search has already been done, and the answer is available
	int cacheLoc = (x + y*voxelSpace.x_size + z*(voxelSpace.x_size*voxelSpace.y_size))*2;
	if (cacheLoc >= 0 && cacheLoc < m_distanceCacheSize)
	{
		if (&voxelSpace == m_edgeVspace)
			cacheLoc++;
		if (m_distanceCache[cacheLoc] != std::numeric_limits<float>::infinity())
			return m_distanceCache[cacheLoc];
	}
	
	// Not in the cache - so calculate it
	float ret = 500.0;
	char* lookup = s_lookupData;
	while (lookup != s_lookupEnd) // This is lots faster than for (i=0 ; i<size...
	{
		if (voxelSpace.get(x + lookup[0], y + lookup[1], z + lookup[2]))
		{
			ret = ::pow(lookup[0], 2) + ::pow(lookup[1], 2) + ::pow(lookup[2], 2);
			break;
		}
		
		lookup += 3;
	}
	
	// Store it in the cache.  Another thread might have done this already, but it doesn't
	// matter since both threads will get the same answer and we'll only be overwriting the value.
	if (cacheLoc >= 0 && cacheLoc < m_distanceCacheSize)
		m_distanceCache[cacheLoc] = ret;
	
	return 500.0; // TODO: Store this value in the lookup file
}

Mat4 FrameInfo::limbMatrix(Part part, Limb limb, const Params<float>& p) const
{
	const Params<float> params = (p.valid) ? p : (part == LeftLeg ? m_leftLegParams : m_rightLegParams);
	
	// Scale factors
	const float scale1 = 1.0 / qMax(qMax(s_thighModel->extent()[0], s_thighModel->extent()[1]), s_thighModel->extent()[2]);
	const float scale2 = float(m_highest) / 4.0;
	
	Mat4 matrix;
	matrix = HTrans4(-s_thighModel->min()); // Translate origin to min
	matrix = HTrans4(Vec3(-s_thighModel->extent()[0] / 2.0, -s_thighModel->extent()[1] / 2.0, -s_thighModel->extent()[2])) * matrix; // Translate origin to center of the top face
	matrix = HScale4(Vec3(scale1, scale1, scale1)) * matrix; // Scale down to 0-1 space
	matrix = HScale4(Vec3(scale2, scale2, scale2)) * matrix; // Scale up to voxel space
	if (limb == LowerLeg)
	{
		matrix = HRot4(Vec3(1.0, 0.0, 0.0), params.lowerLegTheta - params.thighTheta) * matrix; // Apply lower leg theta rotations
		matrix = HRot4(Vec3(0.0, 1.0, 0.0), params.lowerLegAlpha - params.thighAlpha) * matrix; // Apply lower leg alpha rotations
		matrix = HTrans4(Vec3(0.0, 0.0, -scale2)) * matrix; // Go down the thigh to the lower leg position
	}
	matrix = HRot4(Vec3(1.0, 0.0, 0.0), params.thighTheta) * matrix; // Apply thigh theta rotation
	matrix = HRot4(Vec3(0.0, 1.0, 0.0), params.thighAlpha) * matrix; // Apply thigh alpha rotation
	matrix = HTrans4(Vec3(m_center[0], m_center[1], m_highest / 2.0)) * matrix; // Move to the estimated thigh position
	if (part == RightLeg)
		matrix = HTrans4(Vec3(m_xWidth / 6.0, 0.0, 0.0)) * matrix; // Change to right leg
	else if (part == LeftLeg)
		matrix = HTrans4(Vec3(- m_xWidth / 6.0, 0.0, 0.0)) * matrix; // Change to left leg
	
	return matrix;
}

void FrameInfo::initDistanceCache()
{
	// Init all elements of the cache to +ve infinity
	m_distanceCacheSize = m_vspace->x_size * m_vspace->y_size * m_vspace->z_size * 2;
	m_distanceCache = new float[m_distanceCacheSize];
	
	float* end = m_distanceCache + m_distanceCacheSize;
	float* p = m_distanceCache;
	float inf = std::numeric_limits<float>::infinity();
	
	while (p != end)
		*(p++) = inf;
}

void FrameInfo::save() const
{
	QSettings settings(m_fileName + ".info", QSettings::IniFormat);
	save(settings);
}

void FrameInfo::save(QSettings& settings) const
{
	settings.beginGroup("General");
	settings.setValue("Highest", m_highest);
	settings.setValue("CenterX", m_center[0]);
	settings.setValue("CenterY", m_center[1]);
	settings.setValue("XWidth", m_xWidth);
	settings.endGroup();
	
	settings.beginGroup("LeftLeg");
	m_leftLegParams.save(settings);
	settings.endGroup();
	
	settings.beginGroup("RightLeg");
	m_rightLegParams.save(settings);
	settings.endGroup();
}

void FrameInfo::load()
{
	if (!QFile::exists(m_fileName + ".info"))
		return;
	
	QSettings settings(m_fileName + ".info", QSettings::IniFormat);
	load(settings);
}

void FrameInfo::load(QSettings& settings)
{
	settings.beginGroup("General");
	m_highest = settings.value("Highest").toInt();
	m_center[0] = settings.value("CenterX").toDouble();
	m_center[1] = settings.value("CenterY").toDouble();
	m_xWidth = settings.value("XWidth").toInt();
	settings.endGroup();
	
	settings.beginGroup("LeftLeg");
	m_leftLegParams = Params<float>(settings);
	settings.endGroup();
	
	settings.beginGroup("RightLeg");
	m_rightLegParams = Params<float>(settings);
	settings.endGroup();
}


