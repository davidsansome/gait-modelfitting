#include "analysis.h"

#include <QDebug>

#include <vspace.hh>

Analysis::Analysis(Voxel_Space* vspace)
	: m_vspace(vspace)
{
}

Analysis::~Analysis()
{
}

float2 Analysis::findCenter()
{
	unsigned long long xTotal = 0;
	unsigned long long yTotal = 0;
	unsigned int i = 0;
	
	for (int x=0 ; x<m_vspace->x_size ; ++x)
	{
		for (int y=0 ; y<m_vspace->y_size ; ++y)
		{
			for (int z=0 ; z<m_vspace->z_size ; ++z)
			{
				if (!m_vspace->get(x, y, x))
					continue;
				
				xTotal += x;
				yTotal += y;
				i++;
			}
		}
	}
	
	return floatn<2>(float(xTotal)/i, float(yTotal)/i);
}


