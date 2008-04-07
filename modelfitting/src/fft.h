#ifndef FFT_H
#define FFT_H

#include <complex>
#include <fftw3.h>

class FrameSet;
class CurveFitter;

class Fft
{
public:
	Fft();
	~Fft();
	
	enum Type
	{
		LeftThighAlpha,
		LeftThighTheta,
		LeftLowerAlpha,
		LeftLowerTheta,
		RightThighAlpha,
		RightThighTheta,
		RightLowerAlpha,
		RightLowerTheta
	};
	
	void setFrameSet(FrameSet* frameSet);
	void init(int min = -1, int max = -1);
	void run(Type type);
	
	int resultSize() const { return m_dataSize/2+1; }
	const std::complex<double>* result() const { return m_results; }

private:
	FrameSet* m_frameSet;
	CurveFitter* m_curveFitter;
	
	int m_dataSize;
	int m_dataMin;
	int m_dataMax;
	double* m_data;
	std::complex<double>* m_results;
	fftw_plan m_plan;
};

#endif
