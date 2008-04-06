#ifndef FFTPLOTTER_H
#define FFTPLOTTER_H

#include "graphplotter.h"

#include <fftw3.h>
#include <complex>

class FftPlotter : public GraphPlotter
{
public:
	FftPlotter(QWidget* parent = 0);
	
protected:
	void aboutToShow();
	QString templateName(bool displayOnly) const;
	void plotData(const QString& outFilename);
	void replaceTokens(QByteArray& commands);

private:
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
	
	void initData(Type type);
	void runAndPlot(Type type, const QString& outFilename);
	void plot(const QString& plot, const QString& outFilename);
	
	int m_dataSize;
	double* m_data;
	std::complex<double>* m_results;
	fftw_plan m_plan;
	
	QString m_plot;
};

#endif

