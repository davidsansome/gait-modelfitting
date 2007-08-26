#include "canny.h"
#include "dataunit.h"
#include "gaussian.h"
#include "intensitygradient.h"

#include <QGLFramebufferObject>

Canny::Canny()
	: ChainFilterSet()
{
	m_gaussian = new Gaussian();
	m_intensityGradient = new IntensityGradient();

	addFilterSet(m_gaussian);
	addFilterSet(m_intensityGradient);
}

Canny::~Canny()
{
}

void Canny::init(CGcontext context, CGprofile vertProfile, CGprofile fragProfile, const QSize& imageSize)
{
	m_temp = new DataUnit(new QGLFramebufferObject(imageSize));
	
	ChainFilterSet::init(context, vertProfile, fragProfile, imageSize);
}

void Canny::joinFilterSets()
{
	m_gaussian->setDataUnit(0, m_input);
	m_gaussian->setDataUnit(1, m_temp);

	m_intensityGradient->setDataUnit(0, m_temp);
	m_intensityGradient->setDataUnit(1, m_output);
}


