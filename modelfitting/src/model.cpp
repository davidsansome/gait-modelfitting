#include "model.h"
#include "modelloader.h"

Model::Model(const QString& name)
	: m_name(name)
{
	ModelLoader::instance()->addModel(this);
}

void Model::setVertices(int count, const Vec3* data)
{
	m_vertexCount = count;
	m_vertexData = data;
}
