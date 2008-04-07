#include "fft.h"
#include "frameinfo.h"
#include "frameset.h"
#include "curvefitter.h"

Fft::Fft()
	: m_data(NULL),
	  m_results(NULL),
	  m_frameSet(NULL)
{
	m_curveFitter = new CurveFitter;
}

Fft::~Fft()
{
	delete m_curveFitter;
	fftw_destroy_plan(m_plan);
	fftw_free(m_data);
	fftw_free(m_results);
}

void Fft::setFrameSet(FrameSet* frameSet)
{
	m_frameSet = frameSet;
	m_curveFitter->setFrameSet(frameSet);
}

void Fft::init(int min, int max)
{
	if (m_data != NULL)
	{
		fftw_destroy_plan(m_plan);
		fftw_free(m_data);
		fftw_free(m_results);
	}
	
	if (min == -1 || max == -1)
	{
		m_curveFitter->setFrameSet(m_frameSet);
		FittingResult result(m_curveFitter->doFitting());
		
		float zeroCrossing = result.zeroCrossing();
		while (zeroCrossing < 0.0)
			zeroCrossing += result.period / 2.0;
		
		m_dataMin = ROUND(zeroCrossing);
		m_dataMax = ROUND(zeroCrossing + result.period);
		qDebug() << m_dataMin << "<= t <=" << m_dataMax;
	}
	else
	{
		m_dataMin = min;
		m_dataMax = max;
	}
	
	m_dataSize = m_dataMax - m_dataMin;
	
	m_data = (double*) fftw_malloc(sizeof(double) * m_dataSize);
	m_results = (std::complex<double>*) fftw_malloc(sizeof(std::complex<double>) * (m_dataSize/2 + 1));
	m_plan = fftw_plan_dft_r2c_1d(m_dataSize, m_data, reinterpret_cast<fftw_complex*>(m_results), FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
}

void Fft::run(Type type)
{
	const double* dataEnd = m_data + m_dataSize;
	double* p = m_data;
	
	int i = m_dataMin;
	while (p != dataEnd)
	{
		if (!m_frameSet->hasModelInformation(i))
			*p = 0.0;
		else
		{
			FrameInfo* info = m_frameSet->loadFrame(i, true);
			
			switch (type)
			{
				case LeftThighAlpha:  *p = info->leftLeg().thighAlpha;     break;
				case LeftThighTheta:  *p = info->leftLeg().thighTheta;     break;
				case LeftLowerAlpha:  *p = info->leftLeg().lowerLegAlpha;  break;
				case LeftLowerTheta:  *p = info->leftLeg().lowerLegTheta;  break;
				case RightThighAlpha: *p = info->rightLeg().thighAlpha;    break;
				case RightThighTheta: *p = info->rightLeg().thighTheta;    break;
				case RightLowerAlpha: *p = info->rightLeg().lowerLegAlpha; break;
				case RightLowerTheta: *p = info->rightLeg().lowerLegTheta; break;
			}
			
			delete info;
		}
		
		i++;
		p++;
	}
	
	fftw_execute(m_plan);
}

