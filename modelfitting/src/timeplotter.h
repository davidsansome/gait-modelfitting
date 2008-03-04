#ifndef TIMEPLOTTER_H
#define TIMEPLOTTER_H

#include "graphplotter.h"

class TimePlotter : public GraphPlotter
{
public:
	TimePlotter(QWidget* parent = 0);
	
protected:
	QString templateName(bool displayOnly) const;
	void plotData(const QString& outFilename);
	void replaceTokens(QByteArray& commands);
};

#endif

