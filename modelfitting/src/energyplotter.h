#ifndef ENERGYPLOTTER_H
#define ENERGYPLOTTER_H

#include "graphplotter.h"

class EnergyPlotter : public GraphPlotter
{
public:
	EnergyPlotter(FrameModel* model, QWidget* parent = 0);
	
protected:
	QString templateName(bool displayOnly) const;
	void plotData(const QString& outFilename);
	void replaceTokens(QByteArray& commands);

private:
	void plotData(int limb, const QString& outFilename);
	QTextStream& writeThighData(Part part, const Params<float>& initialParams, QTextStream& s);
	QTextStream& writeLowerLegData(Part part, const Params<float>& initialParams, QTextStream& s);
	
	int m_limb;
};

#endif

