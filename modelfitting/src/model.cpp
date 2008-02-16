#include "model.h"
#include "modelloader.h"

#include <QDebug>
#include <GL/gl.h>
#include <svl/SVLgl.h>

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

void Model::drawPoints() const
{
	glBegin(GL_POINTS);
		const Vertex* vertex = m_vertexData;
		for (int i=0 ; i<m_vertexCount ; i++)
		{
			//glColor(m_materialData[vertex->mat].color);
			glVertex(vertex->pos);
			
			vertex++;
		}
	glEnd();
}
