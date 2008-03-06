#include "errorcorrection.h"
#include "frameset.h"
#include "frameinfo.h"

#include <limits>
#include <cmath>

#include <QtDebug>

ErrorCorrection::ErrorCorrection(QWidget* parent)
	: QDialog(parent),
	  m_frameSet(NULL),
	  m_data(NULL)
{
	m_ui.setupUi(this);
	
	connect(m_ui.buttonBox, SIGNAL(accepted()), SLOT(okClicked()));
	
	m_settings.beginGroup("ErrorCorrection");
}

ErrorCorrection::~ErrorCorrection()
{
	delete[] m_data;
}

void ErrorCorrection::setFrameSet(FrameSet* frameSet)
{
	m_frameSet = frameSet;
	int min = std::numeric_limits<int>::max();
	int max = std::numeric_limits<int>::min();
	
	for (int i=0 ; i<m_frameSet->count() ; ++i)
	{
		if (!m_frameSet->hasModelInformation(i))
			continue;
		min = qMin(min, i);
		max = qMax(max, i);
	}
	
	m_ui.tMin->setRange(min, max);
	m_ui.tMax->setRange(min, max);
	m_ui.tMin->setValue(m_settings.value("TMin", min).toInt());
	m_ui.tMax->setValue(m_settings.value("TMax", max).toInt());
}

void ErrorCorrection::okClicked()
{
	if (m_ui.tMax->value() < m_ui.tMax->value())
	{
		reject();
		return;
	}
	
	initData();
	
	// TODO: Mapreduce maybe
	float minEnergy = std::numeric_limits<float>::max();
	float minPeriod = 0.0;
	float minPhase = 0.0;
	for (float period=30.0 ; period<=40.0 ; period+=0.1)
	{
		for (float phase=0.0 ; phase<=2.0*M_PI ; phase+=(2.0*M_PI)/100.0)
		{
			float e = energy(period, phase);
			if (e >= minEnergy)
				continue;
			
			minEnergy = e;
			minPeriod = period;
			minPhase = phase;
		}
	}
	
	qDebug() << "Minimum period" << minPeriod << "phase" << minPhase << "with energy" << minEnergy;
	
	delete[] m_data;
	m_data = NULL;
	
	m_settings.setValue("TMin", m_ui.tMin->value());
	m_settings.setValue("TMax", m_ui.tMax->value());
	accept();
}

void ErrorCorrection::initData()
{
	delete[] m_data;
	int size = 2 * (m_ui.tMax->value() - m_ui.tMin->value() + 1);
	m_data = new float[size];
	m_dataEnd = m_data + size;
	
	float* p = m_data;
	
	int i = m_ui.tMin->value();
	while (p != m_dataEnd)
	{
		if (!m_frameSet->hasModelInformation(i))
		{
			p[0] = 0.0;
			p[1] = 0.0;
		}
		else
		{
			FrameInfo* info = m_frameSet->loadFrame(i, true);
			p[0] = info->leftLeg().thighTheta;
			p[1] = info->rightLeg().thighTheta;
			delete info;
		}
		
		i++;
		p+=2;
	}
}

float ErrorCorrection::energy(float period, float phase)
{
	float* p = m_data;
	float accum = 0.0;
	int t = m_ui.tMin->value();
	while (p != m_dataEnd)
	{
		float sinLeft = 0.05 + 0.55 * sinf((2.0 * M_PI * t) / period + phase);
		float sinRight = 0.05 + 0.55 * sinf((2.0 * M_PI * t) / period + (phase + M_PI)); // Legs are out of phase by PI
		
		accum += pow(sinLeft - p[0], 2) + pow(sinRight - p[1], 2);
		
		t++;
		p+=2;
	}
	
	return accum;
}



