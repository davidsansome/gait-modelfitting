#include "timeplotter.h"

#include <QFileInfo>
#include <QDir>

#define ROUND(x) int((x) + ((x) > 0.0 ? 0.5 : -0.5))

TimePlotter::TimePlotter(QWidget* parent)
	: GraphPlotter(parent)
{
	m_ui.graphsGroupBox->hide();
	m_ui.displayOnly->hide();
	m_ui.filesGroupBox->hide();
}

QString TimePlotter::templateName(bool displayOnly) const
{
	if (displayOnly)
		return ":showtime.g";
	return ":savetime.g";
}

void TimePlotter::plotData(const QString& outFilename, const QString& extension)
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
		cols << settings.value("ThighTheta").toString();
		settings.endGroup();
		
		s << cols.join(" ") << "\n";
	}
	
	saveGraph(outFilename);
}

void TimePlotter::replaceTokens(QByteArray& commands)
{
	
}
