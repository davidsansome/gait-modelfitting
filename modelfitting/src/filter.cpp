#include "filter.h"

#include <QFile>
#include <QDebug>
#include <QDataStream>

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
}

Filter::~Filter()
{
	delete[] m_data;
}

float Filter::get(int x, int y, int z) const
{
	return m_data[z*m_zSize + y*m_ySize + x];
}

