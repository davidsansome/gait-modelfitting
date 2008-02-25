#include "graphplotter.h"

#include <QTemporaryFile>
#include <QMessageBox>
#include <QProcess>
#include <QFileInfo>
#include <QFileDialog>

GraphPlotter::GraphPlotter(QObject* parent)
	: QObject(parent),
	  m_limb(0),
	  m_info(NULL)
{
	m_settings.beginGroup("GraphPlotter");
	m_saveDirectory = m_settings.value("SaveDir", QDir::homePath()).toString();
}

GraphPlotter::~GraphPlotter()
{
}

void GraphPlotter::display()
{
	if (m_info == NULL || !m_info->hasModelInformation())
	{
		QMessageBox::warning(NULL, "Error", "You need to calculate the frame's model information first");
		return;
	}
	
	plot(":showgraph.g");
}

void GraphPlotter::save()
{
	if (m_info == NULL || !m_info->hasModelInformation())
	{
		QMessageBox::warning(NULL, "Error", "You need to calculate the frame's model information first");
		return;
	}
	
	QPair<QString, QString> ret(getFilename());
	if (ret.first.isNull())
		return;
	
	plot(":savegraph.g", ret.first, ret.second);
}

void GraphPlotter::plot(const QString& templateFilename, const QString& outFilename, const QString& termType)
{
	QTemporaryFile data;
	data.open();
	QTextStream s(&data);
	
	switch (m_limb)
	{
	case 0: writeThighData   (LeftLeg,  m_info->leftLeg(),  s); break;
	case 1: writeThighData   (RightLeg, m_info->rightLeg(), s); break;
	case 2: writeLowerLegData(LeftLeg,  m_info->leftLeg(),  s); break;
	case 3: writeLowerLegData(RightLeg, m_info->rightLeg(), s); break;
	}
	
	QFile templateFile(templateFilename);
	templateFile.open(QIODevice::ReadOnly);
	QByteArray commands(templateFile.readAll());
	
	commands.replace("__DATA_FILENAME__", data.fileName().toAscii());
	commands.replace("__OUT_FILENAME__", outFilename.toAscii());
	commands.replace("__TERM_TYPE__", termType.toAscii());
	commands.replace("__VOXEL_FILENAME__", QFileInfo(m_info->filename()).fileName().toAscii());
	commands.replace("__LIMB__", limbName(m_limb).toAscii());
	qDebug() << commands;
	
	data.close();
	
	QProcess gnuplot;
	gnuplot.setProcessChannelMode(QProcess::MergedChannels);
	
	gnuplot.start("gnuplot");
	gnuplot.write(commands);
	
	gnuplot.waitForFinished();
	
	qDebug() << gnuplot.readAll();
}

QTextStream& GraphPlotter::writeThighData(Part part, const Params& initialParams, QTextStream& s)
{
	Params p(initialParams);
	
	for (float ta=-ALPHA_RANGE ; ta<ALPHA_RANGE ; ta+=(2.0*ALPHA_RANGE)/ALPHA_RESOLUTION)
	{
		for (float tt=-THETA_RANGE ; tt<THETA_RANGE ; tt+=(2.0*THETA_RANGE)/THETA_RESOLUTION)
		{
			p.thighAlpha = ta;
			p.thighTheta = tt;
			
			s << tt << " " << ta << " " << m_info->result(p, part) << "\n";
		}
		s << "\n";
	}
	
	return s;
}

QTextStream& GraphPlotter::writeLowerLegData(Part part, const Params& initialParams, QTextStream& s)
{
	Params p(initialParams);
	
	for (float la=-ALPHA_RANGE ; la<ALPHA_RANGE ; la+=(2.0*ALPHA_RANGE)/ALPHA_RESOLUTION)
	{
		for (float lt=-THETA_RANGE ; lt<THETA_RANGE ; lt+=(2.0*THETA_RANGE)/THETA_RESOLUTION)
		{
			p.lowerLegAlpha = la;
			p.lowerLegTheta = lt;
			
			s << lt << " " << la << " " << m_info->result(p, part) << "\n";
			qDebug() << lt << " " << la << " " << m_info->result(p, part) << "\n";
		}
		s << "\n";
	}
	
	return s;
}

QString GraphPlotter::limbName(int t)
{
	switch (t)
	{
		case 0: return "Left thigh";
		case 1: return "Right thigh";
		case 2: return "Left lower leg";
		case 3: return "Right lower leg";
	}
}

QPair<QString, QString> GraphPlotter::getFilename()
{
	QString filters = "PNG image (*.png);;"
	                  "JPEG image (*.jpeg *.jpg);;"
	                  "Postscript document (*.ps);;"
	                  "PDF document (*.pdf);;"
	                  "LaTeX document (*.tex)";
	
	QString outFilename;
	QString termType;
	while (true)
	{
		outFilename = QFileDialog::getSaveFileName(NULL, "Save graph", m_saveDirectory, filters);
		if (outFilename.isNull())
			return QPair<QString, QString>();
		
		if      (outFilename.endsWith(".png")) termType = "png";
		else if (outFilename.endsWith(".ps"))  termType = "postscript";
		else if (outFilename.endsWith(".pdf")) termType = "pdf";
		else if (outFilename.endsWith(".tex")) termType = "latex";
		else if (outFilename.endsWith(".jpg") || outFilename.endsWith(".jpeg")) termType = "jpeg";
		else
		{
			QMessageBox::warning(NULL, "Error", "Please use a supported file extension");
			continue;
		}
		break;
	}
	m_saveDirectory = QFileInfo(outFilename).path();
	m_settings.setValue("SaveDir", m_saveDirectory);
	
	return QPair<QString, QString>(outFilename, termType);
}

