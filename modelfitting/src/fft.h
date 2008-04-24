#ifndef FFT_H
#define FFT_H

#include <complex>
#include <QModelIndex>
#include <fftw3.h>

class FrameModel;
class FrameModelFilter;
class CurveFitter;

class Fft
{
public:
	Fft(FrameModel* model);
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
	
	void setFrameSet(const QModelIndex& frameSet);
	void init(int min = -1, int max = -1);
	void run(Type type);
	
	int resultSize() const { return m_dataSize/2+1; }
	const std::complex<double>* result() const { return m_results; }
	
	int dataMin() const { return m_dataMin; }
	int dataMax() const { return m_dataMax; }
	FrameModelFilter* filter() const { return m_filter; }

private:
	QModelIndex m_index;
	FrameModel* m_model;
	FrameModelFilter* m_filter;
	
	CurveFitter* m_curveFitter;
	
	int m_dataSize;
	int m_dataMin;
	int m_dataMax;
	double* m_data;
	std::complex<double>* m_results;
	fftw_plan m_plan;
};

#endif
