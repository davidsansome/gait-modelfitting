#include "filter.h"

#include <QFile>
#include <QDebug>
#include <QDataStream>

#include <GL/glu.h>

Filter::Filter(const QString& fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
		qFatal("Failed to open file %s\n", fileName.toAscii().data());
	
	QDataStream s(&file);
	s.setByteOrder(QDataStream::BigEndian);
	
	// Read the filter size
	s >> m_xSize >> m_ySize >> m_zSize;
	
	// Read the data
	m_data = new float[m_xSize * m_ySize * m_zSize];
	for (int z=0 ; z<m_zSize ; ++z)
		for (int y=0 ; y<m_ySize ; ++y)
			for (int x=0 ; x<m_xSize ; ++x)
				s >> m_data[z*m_zSize + y*m_ySize + x];
	
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_3D, m_texture);
	gluBuild3DMipmaps(GL_TEXTURE_3D, GL_RGBA, m_xSize, m_ySize, m_zSize, GL_RED, GL_FLOAT, m_data);
}

Filter::~Filter()
{
	glDeleteTextures(1, &m_texture);
	delete[] m_data;
}

float Filter::get(int x, int y, int z) const
{
	return m_data[z*m_zSize + y*m_ySize + x];
}

void Filter::bind(int texUnit)
{
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glEnable(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, m_texture);
}

void Filter::unbind(int texUnit)
{
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glDisable(GL_TEXTURE_3D);
}

