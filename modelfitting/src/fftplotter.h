#ifndef FFTPLOTTER_H
#define FFTPLOTTER_H

#include "graphplotter.h"
#include "fft.h"

class FftPlotter : public GraphPlotter
{
public:
	FftPlotter(QWidget* parent = 0);
	~FftPlotter();
	
protected:
	void aboutToShow();
	QString templateName(bool displayOnly) const;
	void plotData(const QString& outFilename);
	void replaceTokens(QByteArray& commands);

private:
	void runAndPlot(Fft::Type type, const QString& outFilename);
	void plot(const QString& plot, const QString& outFilename);
	
	Fft* m_fft;
	
	QString m_plot;
};

#endif

