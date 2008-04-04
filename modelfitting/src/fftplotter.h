#ifndef FFTPLOTTER_H
#define FFTPLOTTER_H

#include "graphplotter.h"

class FftPlotter : public GraphPlotter
{
public:
	FftPlotter(QWidget* parent = 0);
	
protected:
	void aboutToShow();
	QString templateName(bool displayOnly) const;
	void plotData(const QString& outFilename);
	void replaceTokens(QByteArray& commands);
};

#endif

