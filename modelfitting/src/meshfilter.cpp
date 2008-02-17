#include "meshfilter.h"
#include "modelloader.h"
#include "frameinfo.h"
#include "types.h"
#include "model.h"

#include <QFile>
#include <QDataStream>
#include <limits>

MeshFilter::MeshFilter(const QString& modelName, const QString& lookupTableName)
{
	// Get the mesh model
	m_model = ModelLoader::instance()->model(modelName);
	
	// Get the lookup table
	QFile lookupTable(lookupTableName);
	lookupTable.open(QIODevice::ReadOnly);
	QDataStream s(&lookupTable);
	
	s >> m_lookupElements;
	m_lookupData = new char[m_lookupElements * 3];
	s.readRawData(m_lookupData, m_lookupElements * 3);
	
	qDebug() << "Read" << m_lookupElements << "elements from lookup table";
}

MeshFilter::~MeshFilter()
{
	delete[] m_lookupData;
}

Mat4 MeshFilter::matrix(const FrameInfo* info, Vec vec) const
{
	// Scale factors
	const float scale1 = 1.0 / qMax(qMax(m_model->extent()[0], m_model->extent()[1]), m_model->extent()[2]);
	const float scale2 = float(info->highestPoint()) / 4.0;
	
	Mat4 matrix;
	matrix = HTrans4(-m_model->min()); // Translate origin to min
	matrix = HTrans4(Vec3(-m_model->extent()[0] / 2.0, -m_model->extent()[1] / 2.0, -m_model->extent()[2])) * matrix; // Translate origin to center of the top face
	matrix = HRot4(Vec3(1.0, 0.0, 0.0), vec[0]) * matrix; // Apply thigh theta rotation
	matrix = HRot4(Vec3(0.0, 1.0, 0.0), vec[1]) * matrix; // Apply thigh alpha rotation
	matrix = HScale4(Vec3(scale1, scale1, scale1)) * matrix; // Scale down to 0-1 space
	matrix = HScale4(Vec3(scale2, scale2, scale2)) * matrix; // Scale up to voxel space
	matrix = HTrans4(Vec3(info->center()[0], info->center()[1], info->highestPoint() / 2.0)) * matrix; // Move to the estimated thigh position
	matrix = HTrans4(Vec3(info->xWidth() / 6.0, 0.0, 0.0)) * matrix; // Change to right leg
	
	return matrix;
}

Vec MeshFilter::correlate(const FrameInfo* info)
{
	// Copy the voxel space and do edge detection on it
	Voxel_Space edge(*(info->vspace())); // Ceep copy
	edge.edge_detect();
	
	const int edgeMaterial = (len(m_model->materialData()[0].color) > 0.7) ? 0 : 1;
	const int internalMaterial = (len(m_model->materialData()[1].color) > 0.7) ? 0 : 1;
	
	Vec minVec(2, 0.0, 0.0);
	float minEnergy = std::numeric_limits<float>::infinity();
	for (float alpha=-M_PI_4 ; alpha<M_PI_4 ; alpha+=M_PI_2/40)
	{
		for (float theta=-M_PI_4 ; theta<M_PI_4 ; theta+=M_PI_2/40)
		{
			Vec vec(2, theta, alpha);
			Mat4 mat = matrix(info, vec);
			
			float energy = 0.0;
			
			const Vertex* vertex = m_model->vertexData();
			for (int i=0 ; i<m_model->vertexCount() ; ++i)
			{
				const Vec3 pos(proj(mat * vertex->pos));
				int x = int(pos[0]); // These get floored, that's ok
				int y = int(pos[1]);
				int z = int(pos[2]);
				
				if (vertex->mat == edgeMaterial) // Search for nearby edge voxels
					energy += doSearch(edge, x, y, z);
				else // Search for nearby filled voxels
					energy += doSearch(*info->vspace(), x, y, z);
				
				vertex++;
			}
			
			if (energy < minEnergy)
			{
				minEnergy = energy;
				minVec = vec;
			}
			qDebug() << "vec" << vec << "energy" << energy;
		}
	}
	
	return minVec;
}

float MeshFilter::doSearch(const Voxel_Space& voxelSpace, int x, int y, int z)
{
	char* lookup = m_lookupData;
	for (int i=0 ; i<m_lookupElements ; i++)
	{
		if (voxelSpace.get(x + lookup[0], y + lookup[1], z + lookup[2]))
			return pow(lookup[0], 2) + pow(lookup[1], 2) + pow(lookup[2], 2);
		
		lookup += 3;
	}
	
	return 500.0; // TODO: Store this value in the lookup file
}

