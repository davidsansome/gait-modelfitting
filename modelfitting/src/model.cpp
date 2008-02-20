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

void Model::setTriangles(int count, const Triangle* data)
{
	m_triangleCount = count;
	m_triangleData = data;
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

void Model::draw() const
{
	// TODO: VBOs?
	glBegin(GL_TRIANGLES);
		const Triangle* triangle = m_triangleData;
		for (int i=0 ; i<m_triangleCount ; i++)
		{
			glColor(m_materialData[triangle->mat].color);
			glVertex(m_vertexData[triangle->v1].pos);
			glVertex(m_vertexData[triangle->v2].pos);
			glVertex(m_vertexData[triangle->v3].pos);
			
			triangle++;
		}
	glEnd();
}
