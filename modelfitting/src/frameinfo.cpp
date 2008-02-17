#include "frameinfo.h"
#include "convolution.h"

#include <QDebug>
#include <QTime>

#include <GL/glu.h>

#include <vspace.hh>

FrameInfo::FrameInfo(Voxel_Space* vspace)
	: m_vspace(vspace),
	  m_texture(0)
{
}

FrameInfo::~FrameInfo()
{
	if (m_texture != 0)
		glDeleteTextures(1, &m_texture);
}

void FrameInfo::analyse()
{
	QTime t;
	t.start();
	
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
	m_center = floatn<2>(float(xTotal)/i, float(yTotal)/i);
	m_xWidth = maxX - minX;
	
	qDebug() << "Finding highest and center took" << t.restart() << "ms";
	
	// Convert vspace into format suitable for loading into opengl
	uchar* data = new uchar[m_vspace->x_size * m_vspace->y_size * m_vspace->z_size];
	uchar* d = data;
	
	for (int z=0 ; z<m_vspace->z_size ; ++z)
		for (int y=0 ; y<m_vspace->y_size ; ++y)
			for (int x=0 ; x<m_vspace->x_size ; ++x)
				*(d++) = m_vspace->get(x, y, z) ? 0xFF : 0x00;
	
	// Load it into an opengl texture
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_3D, m_texture);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, m_vspace->x_size, m_vspace->y_size, m_vspace->z_size, 0, GL_RED, GL_UNSIGNED_BYTE, data);
	
	delete[] data;
	
	qDebug() << "Building 3D texture took" << t.restart() << "ms and was" << m_vspace->x_size * m_vspace->y_size * m_vspace->z_size << "bytes";
}

void FrameInfo::bindTexture(int texUnit)
{
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glEnable(GL_TEXTURE_3D);
	glBindTexture(GL_TEXTURE_3D, m_texture);
}


