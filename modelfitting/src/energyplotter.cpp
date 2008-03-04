#include "energyplotter.h"

#include <QFileInfo>

#define ROUND(x) int((x) + ((x) > 0.0 ? 0.5 : -0.5))

EnergyPlotter::EnergyPlotter(QWidget* parent)
	: GraphPlotter(parent)
{
	m_ui.stackedWidget->setCurrentWidget(m_ui.energyPage);
}

QString EnergyPlotter::templateName(bool displayOnly) const
{
	if (displayOnly)
		return ":showgraph.g";
	return ":savegraph.g";
}

void EnergyPlotter::plotData(const QString& outFilename)
{
	if (m_ui.leftThigh->isChecked())     plotData(0, outFilename + "-leftthigh");
	if (m_ui.rightThigh->isChecked())    plotData(1, outFilename + "-rightthigh");
	if (m_ui.leftLowerLeg->isChecked())  plotData(2, outFilename + "-leftlower");
	if (m_ui.rightLowerLeg->isChecked()) plotData(3, outFilename + "-rightlower");
}

void EnergyPlotter::plotData(int limb, const QString& outFilename)
{
	QTextStream& s = openTempFile();
	
	switch (limb)
	{
		case 0: writeThighData   (LeftLeg,  info()->leftLeg(),  s); break;
		case 1: writeThighData   (RightLeg, info()->rightLeg(), s); break;
		case 2: writeLowerLegData(LeftLeg,  info()->leftLeg(),  s); break;
		case 3: writeLowerLegData(RightLeg, info()->rightLeg(), s); break;
	}
	
	m_limb = limb;
	saveGraph(outFilename);
}

void EnergyPlotter::replaceTokens(QByteArray& commands)
{
	QString limbName;
	switch (m_limb)
	{
		case 0: limbName = "Left thigh";      break;
		case 1: limbName = "Right thigh";     break;
		case 2: limbName = "Left lower leg";  break;
		case 3: limbName = "Right lower leg"; break;
	}
	
	commands.replace("__LIMB__", limbName.toAscii());
	commands.replace("__VOXEL_FILENAME__", QFileInfo(info()->filename()).fileName().toAscii());
}

QTextStream& EnergyPlotter::writeThighData(Part part, const Params<float>& initialParams, QTextStream& s)
{
	Params<int> p(0,
	              0,
	              ROUND(initialParams.lowerLegAlpha / ALPHA_STEP),
	              ROUND(initialParams.lowerLegTheta / THETA_STEP));
	
	for (p.thighAlpha=-ALPHA_RESOLUTION ; p.thighAlpha<=ALPHA_RESOLUTION ; p.thighAlpha++)
	{
		for (p.thighTheta=-THETA_RESOLUTION ; p.thighTheta<=THETA_RESOLUTION ; p.thighTheta++)
		{
			s << float(p.thighTheta) * THETA_STEP << " " << float(p.thighAlpha) * ALPHA_STEP << " " << info()->result(p, part) << "\n";
		}
		s << "\n";
	}
	
	return s;
}

QTextStream& EnergyPlotter::writeLowerLegData(Part part, const Params<float>& initialParams, QTextStream& s)
{
	Params<int> p(ROUND(initialParams.thighAlpha / ALPHA_STEP),
	              ROUND(initialParams.thighTheta / THETA_STEP),
	              0,
	              0);
	
	for (p.lowerLegAlpha=-ALPHA_RESOLUTION ; p.lowerLegAlpha<=ALPHA_RESOLUTION ; p.lowerLegAlpha++)
	{
		for (p.lowerLegTheta=-THETA_RESOLUTION ; p.lowerLegTheta<=THETA_RESOLUTION ; p.lowerLegTheta++)
		{
			s << float(p.lowerLegTheta) * THETA_STEP << " " << float(p.lowerLegAlpha) * ALPHA_STEP << " " << info()->result(p, part) << "\n";
		}
		s << "\n";
	}
	
	return s;
}
