#include "timeplotter.h"

#include <QFileInfo>
#include <QDir>

#define ROUND(x) int((x) + ((x) > 0.0 ? 0.5 : -0.5))

TimePlotter::TimePlotter(QWidget* parent)
	: GraphPlotter(parent)
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
	
	QRegExp timeRegexp("(\\d+)");
	
	QDir dir(QFileInfo(info()->filename()).dir());
	QStringList rows(dir.entryList(QStringList() << "*.info", QDir::Files | QDir::NoDotAndDotDot, QDir::Name));
	foreach (QString row, rows)
	{
		// Should really put the frame number inside the info file
		if (timeRegexp.indexIn(row) == -1)
			continue;
		
		QStringList cols;
		cols << timeRegexp.cap(1);
		
		QSettings settings(dir.path() + QDir::separator() + row, QSettings::IniFormat);
		settings.beginGroup("LeftLeg");
		cols << settings.value("ThighAlpha").toString();
		cols << settings.value("ThighTheta").toString();
		cols << settings.value("LowerLegAlpha").toString();
		cols << settings.value("LowerLegTheta").toString();
		settings.endGroup();
		settings.beginGroup("RightLeg");
		cols << settings.value("ThighAlpha").toString();
		cols << settings.value("ThighTheta").toString();
		cols << settings.value("LowerLegAlpha").toString();
		cols << settings.value("LowerLegTheta").toString();
		settings.endGroup();
		
		s << cols.join(" ") << "\n";
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
