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
	
	QDataStream stream(&file);
	stream.setByteOrder(QDataStream::BigEndian);
	
	// Read the filter size
	stream >> m_xSize >> m_ySize >> m_zSize;
	qDebug() << m_xSize << m_ySize << m_zSize;
	
	// Read the data
	float* data = new float[m_xSize * m_ySize * m_zSize];
	for (int z=0 ; z<m_zSize ; ++z)
		for (int y=0 ; y<m_ySize ; ++y)
			for (int x=0 ; x<m_xSize ; ++x)
				stream >> data[z*m_ySize*m_xSize + y*m_xSize + x];

	// Load it into an opengl texture
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_3D, m_texture);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	gluBuild3DMipmaps(GL_TEXTURE_3D, GL_RGB, m_xSize, m_ySize, m_zSize, GL_RED, GL_FLOAT, data);
	delete data;
}

Filter::~Filter()
{
	glDeleteTextures(1, &m_texture);
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

