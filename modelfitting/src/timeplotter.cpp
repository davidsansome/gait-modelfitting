#include "timeplotter.h"
#include "frameset.h"

#include <QFileInfo>
#include <QDir>

#define ROUND(x) int((x) + ((x) > 0.0 ? 0.5 : -0.5))

TimePlotter::TimePlotter(QWidget* parent)
	: GraphPlotter("Params-over-time plotter", parent)
{
	m_ui.stackedWidget->setCurrentWidget(m_ui.timePage);
}

QString TimePlotter::templateName(bool displayOnly) const
{
	if (displayOnly)
		return ":showtime.g";
	return ":savetime.g";
}

void TimePlotter::plotData(const QString& outFilename)
{
	QTextStream& s = openTempFile();
	
	for (int i=0 ; i<frameInfo()->frameSet()->count() ; ++i)
	{
		if (!frameInfo()->frameSet()->hasModelInformation(i))
			continue;
		
		FrameInfo* info = frameInfo()->frameSet()->loadFrame(i, true);
		
		QStringList cols;
		cols << QString::number(i);
		cols << QString::number(info->leftLeg().thighAlpha, 'f');
		cols << QString::number(info->leftLeg().thighTheta, 'f');
		cols << QString::number(info->leftLeg().lowerLegAlpha, 'f');
		cols << QString::number(info->leftLeg().lowerLegTheta, 'f');
		cols << QString::number(info->rightLeg().thighAlpha, 'f');
		cols << QString::number(info->rightLeg().thighTheta, 'f');
		cols << QString::number(info->rightLeg().lowerLegAlpha, 'f');
		cols << QString::number(info->rightLeg().lowerLegTheta, 'f');
		
		s << cols.join(" ") << "\n";
		
		delete info;
	}
	
	saveGraph(outFilename);
}

void TimePlotter::replaceTokens(QByteArray& commands)
{
	QStringList plots;
	
	if (m_ui.leftThighAlpha->isChecked()) plots << "'__DATA_FILENAME__' using 1:2 title \"Left thigh alpha\"";
	if (m_ui.leftThighTheta->isChecked()) plots << "'__DATA_FILENAME__' using 1:3 title \"Left thigh theta\"";
	if (m_ui.leftLowerAlpha->isChecked()) plots << "'__DATA_FILENAME__' using 1:4 title \"Left lower leg alpha\"";
	if (m_ui.leftLowerTheta->isChecked()) plots << "'__DATA_FILENAME__' using 1:5 title \"Left lower leg theta\"";
	if (m_ui.rightThighAlpha->isChecked()) plots << "'__DATA_FILENAME__' using 1:6 title \"Right thigh alpha\"";
	if (m_ui.rightThighTheta->isChecked()) plots << "'__DATA_FILENAME__' using 1:7 title \"Right thigh theta\"";
	if (m_ui.rightLowerAlpha->isChecked()) plots << "'__DATA_FILENAME__' using 1:8 title \"Right lower leg alpha\"";
	if (m_ui.rightLowerTheta->isChecked()) plots << "'__DATA_FILENAME__' using 1:9 title \"Right lower leg theta\"";
	
	commands.replace("__PLOTS__", plots.join(", ").toAscii());
}
