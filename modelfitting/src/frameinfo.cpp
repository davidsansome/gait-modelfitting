#include "frameinfo.h"
#include "model.h"
#include "modelloader.h"

#include <QDebug>
#include <QTime>
#include <QFutureWatcher>
#include <QFile>
#include <QtConcurrentMap>

#include <GL/glu.h>

#include <vspace.hh>
#include <mesh.hh>

const Model* FrameInfo::s_thighModel = NULL;
quint32 FrameInfo::s_lookupElements = 0;
char* FrameInfo::s_lookupData = NULL;

#define ALPHA_RESOLUTION 10
#define THETA_RESOLUTION 20


Params::Params(float ta, float tt, float la, float lt)
	: valid(true),
	  thighAlpha(ta),
	  thighTheta(tt),
	  lowerLegAlpha(la),
	  lowerLegTheta(lt)
{
}

Params::Params(const Params& other)
	: valid(true),
	  thighAlpha(other.thighAlpha),
	  thighTheta(other.thighTheta),
	  lowerLegAlpha(other.lowerLegAlpha),
	  lowerLegTheta(other.lowerLegTheta)
{
}




FrameInfo::FrameInfo(const QString& filename)
{
	m_vspace = new Voxel_Space(filename.toAscii().data());
	m_mesh = new Mesh(*m_vspace);
	m_mesh->draw_init(true);
	
	m_edgeVspace = new Voxel_Space(*m_vspace);
	m_edgeVspace->edge_detect();
	
	m_leftLegWatcher = new QFutureWatcher<ReduceType>(this);
	m_rightLegWatcher = new QFutureWatcher<ReduceType>(this);
	connect(m_leftLegWatcher, SIGNAL(finished()), SLOT(leftLegFinished()));
	connect(m_rightLegWatcher, SIGNAL(finished()), SLOT(rightLegFinished()));
	
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
	}
}

FrameInfo::~FrameInfo()
{
	m_mesh->draw_destroy();
	delete m_mesh;
	delete m_vspace;
	delete m_edgeVspace;
}

void FrameInfo::leftLegFinished()
{
	if (m_leftLegWatcher->isCanceled())
		return;
	
	m_leftLegParams = m_leftLegWatcher->future().result().first;
}

void FrameInfo::rightLegFinished()
{
	if (m_rightLegWatcher->isCanceled())
		return;
	
	m_rightLegParams = m_rightLegWatcher->future().result().first;
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
	
	QList<MapReduceOperation> ret;
	
	// Construct list of potential parameters
	QList<MapType> params;
	for (float ta=-M_PI/8.0 ; ta<M_PI/8.0 ; ta+=M_PI_4/ALPHA_RESOLUTION)
		for (float tt=-M_PI_4 ; tt<M_PI_4 ; tt+=M_PI_2/THETA_RESOLUTION)
			for (float la=-M_PI/8.0 ; la<M_PI/8.0 ; la+=M_PI_4/ALPHA_RESOLUTION)
				for (float lt=-M_PI_4 ; lt<M_PI_4 ; lt+=M_PI_2/THETA_RESOLUTION)
					params << MapType(Params(ta, tt, la, lt), MapArgs(this, LeftLeg));
	
	// Start the mapreduce
	QFuture<ReduceType> future = QtConcurrent::mappedReduced(params, correlateMap, correlateReduce);
	m_leftLegWatcher->setFuture(future);
	ret << MapReduceOperation("Left leg", future);
	
	params.clear();
	for (float ta=-M_PI/8.0 ; ta<M_PI/8.0 ; ta+=M_PI_4/ALPHA_RESOLUTION)
		for (float tt=-M_PI_4 ; tt<M_PI_4 ; tt+=M_PI_2/THETA_RESOLUTION)
			for (float la=-M_PI/8.0 ; la<M_PI/8.0 ; la+=M_PI_4/ALPHA_RESOLUTION)
				for (float lt=-M_PI_4 ; lt<M_PI_4 ; lt+=M_PI_2/THETA_RESOLUTION)
					params << MapType(Params(ta, tt, la, lt), MapArgs(this, RightLeg));
	
	// Start the mapreduce
	future = QtConcurrent::mappedReduced(params, correlateMap, correlateReduce);
	m_rightLegWatcher->setFuture(future);
	ret << MapReduceOperation("Right leg", future);
	
	return ret;
}

ReduceType correlateMap(const MapType& p)
{
	Params params = p.first;
	FrameInfo* info = p.second.first;
	Part part = p.second.second;
	
	return ReduceType(params, info->energy(part, params));
}

void correlateReduce(ReduceType& result, const ReduceType& intermediate)
{
	if (!result.first.valid || intermediate.second < result.second)
		result = intermediate;
}

float FrameInfo::energy(Part part, const Params& params) const
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
	char* lookup = s_lookupData;
	for (int i=0 ; i<s_lookupElements ; i++)
	{
		if (voxelSpace.get(x + lookup[0], y + lookup[1], z + lookup[2]))
			return ::pow(lookup[0], 2) + ::pow(lookup[1], 2) + ::pow(lookup[2], 2);
		
		lookup += 3;
	}
	
	return 500.0; // TODO: Store this value in the lookup file
}

Mat4 FrameInfo::limbMatrix(Part part, Limb limb, const Params& p) const
{
	const Params params = (p.valid) ? p : (part == LeftLeg ? m_leftLegParams : m_rightLegParams);
	
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
		
		matrix = HRot4(Vec3(1.0, 0.0, 0.0), params.lowerLegTheta) * matrix; // Apply both theta rotations
		matrix = HRot4(Vec3(0.0, 1.0, 0.0), params.lowerLegAlpha) * matrix; // Apply both alpha rotations
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
