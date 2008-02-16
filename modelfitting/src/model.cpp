#include "model.h"
#include "modelloader.h"

#include <QDebug>

Model::Model(const QString& name)
	: m_name(name)
{
	ModelLoader::instance()->addModel(this);
	qDebug() << "Adding model" << name;
}

void Model::setVertices(int count, const Vertex* data)
{
	m_vertexCount = count;
	m_vertexData = data;
}

void Model::setMaterials(int count, const Material* data)
{
	m_materialCount = count;
	m_materialData = data;
}

void Model::setMinMax(const Vec3& min, const Vec3& max)
{
	m_min = min;
	m_max = max;
}
