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

int Analysis::findCenter()
{
	qDebug() << m_vspace->x_size << m_vspace->y_size << m_vspace->z_size;
}


