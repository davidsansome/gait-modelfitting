#include "meshfilter.h"
#include "modelloader.h"
#include "frameinfo.h"
#include "types.h"
#include "model.h"

#include <QFile>
#include <QDataStream>

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
	
	// Calculate the transformation matrix
	m_mat = HTrans4(-m_model->min()); // Translate origin to min
	m_mat = HTrans4(Vec3(m_model->extent()[0] / 2.0, m_model->extent()[1] / 2.0, -m_model->extent()[2])) * m_mat; // Translate origin to center of the top face
	m_mat = HScale4(Vec3(1.0, 1.0, 1.0) / m_model->extent()) * m_mat; // Scale to 0-1 space
	// Scale to voxel space goes here in correlate()
}

MeshFilter::~MeshFilter()
{
	delete[] m_lookupData;
}

void MeshFilter::correlate(FrameInfo* info)
{
	Mat4 matrix = m_mat;
	
	// Do voxel space scaling
	float scale = float(info->highestPoint()) / 4.0;
	matrix = HScale4(Vec3(scale, scale, scale)) * matrix;
	
	// Move to the estimated thigh position
	matrix = HTrans4(Vec3(info->center()[0], info->center()[1], info->highestPoint() / 2.0)) * matrix;
	
	for (int i=0 ; i<m_model->vertexCount() ; ++i)
	{
		qDebug() << i << matrix * m_model->vertexData()[i];
	}
}

