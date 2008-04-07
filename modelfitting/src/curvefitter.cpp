#include "curvefitter.h"
#include "frameinfo.h"
#include "frameset.h"

CurveFitter::CurveFitter()
	: m_frameSet(NULL),
	  m_data(NULL)
{
}

CurveFitter::~CurveFitter()
{
	delete[] m_data;
}

void CurveFitter::initData(int min, int max)
{
	delete[] m_data;
	int size = 2 * (max - min + 1);
	m_data = new float[size];
	m_dataEnd = m_data + size;
	
	float* p = m_data;
	
	int i = min;
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

void FittingResult::calculateEnergy(int min, const float* data, const float* dataEnd)
{
	const float* p = data;
	energy = 0.0;
	int t = min;
	while (p != dataEnd)
	{
		float sinLeft = dcOffset + amplitude * sinf((2.0 * M_PI * t) / period + phase);
		float sinRight = dcOffset + amplitude * sinf((2.0 * M_PI * t) / period + (phase + M_PI)); // Legs are out of phase by PI
		
		energy += pow(sinLeft - p[0], 2) + pow(sinRight - p[1], 2);
		
		t++;
		p+=2;
	}
}

FittingResult CurveFitter::doFitting(int min, int max)
{
	if (min == -1)
		min = 0;
	if (max == -1)
		max = m_frameSet->count() - 1;
	
	initData(min, max);
	
	// TODO: Mapreduce maybe
	FittingResult best;
	FittingResult candidate(0.05, 0.55);
	for (candidate.period=30.0 ; candidate.period<=40.0 ; candidate.period+=0.1)
	{
		for (candidate.phase=0.0 ; candidate.phase<=2.0*M_PI ; candidate.phase+=(2.0*M_PI)/100.0)
		{
			candidate.calculateEnergy(min, m_data, m_dataEnd);
			if (candidate >= best)
				continue;
			
			best = candidate;
		}
	}
	
	delete[] m_data;
	m_data = NULL;
	
	return best;
}

float FittingResult::zeroCrossing() const
{
	return (period * (asin(-dcOffset / amplitude) - phase)) / (2.0 * M_PI);
}

QDebug& operator <<(QDebug& s, const FittingResult& f)
{
	s.nospace() << "FittingResult(period=" << f.period << ", phase=" << f.phase << ", energy=" << f.energy << ")";
	return s;
}
