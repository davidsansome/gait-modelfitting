#include "graphplotter.h"

#include <QTemporaryFile>
#include <QMessageBox>
#include <QProcess>
#include <QFileInfo>
#include <QFileDialog>

#define ROUND(x) int((x) + ((x) > 0.0 ? 0.5 : -0.5))

GraphPlotter::GraphPlotter(QWidget* parent)
	: QDialog(parent),
	  m_info(NULL)
{
	m_ui.setupUi(this);
	
	connect(m_ui.buttonBox, SIGNAL(accepted()), SLOT(okClicked()));
	
	m_settings.beginGroup("GraphPlotter");
	load();
}

GraphPlotter::~GraphPlotter()
{
}

void GraphPlotter::okClicked()
{
	QString templateFilename = (m_ui.displayOnly->isChecked()) ? ":showgraph.g" : ":savegraph.g";
	
	QString extension;
	QString termType;
	switch (m_ui.fileFormat->currentIndex())
	{
		case 0: extension = ".png"; termType = "png";        break;
		case 1: extension = ".jpg"; termType = "jpeg";       break;
		case 2: extension = ".ps";  termType = "postscript"; break;
		case 3: extension = ".pdf"; termType = "pdf";        break;
	}
	
	QString outFilename = m_ui.destDir->text() + QDir::separator() + m_ui.baseFilename->text();
	
	if (!QFileInfo(m_ui.destDir->text()).isDir())
	{
		QMessageBox::warning(this, "Error", "\"" + m_ui.destDir->text() + "\" is not a directory");
		return;
	}
	
	if (m_ui.leftThigh->isChecked())     plot(templateFilename, 0, outFilename + "-leftthigh"  + extension, termType);
	if (m_ui.rightThigh->isChecked())    plot(templateFilename, 1, outFilename + "-rightthigh" + extension, termType);
	if (m_ui.leftLowerLeg->isChecked())  plot(templateFilename, 2, outFilename + "-leftlower"  + extension, termType);
	if (m_ui.rightLowerLeg->isChecked()) plot(templateFilename, 3, outFilename + "-rightlower" + extension, termType);
	
	save();
	accept();
}

void GraphPlotter::load()
{
	m_ui.destDir->setText(m_settings.value("SaveDir", QDir::homePath()).toString());
	m_ui.baseFilename->setText(m_settings.value("BaseName", "graph").toString());
	m_ui.fileFormat->setCurrentIndex(m_settings.value("FileFormat", 0).toInt());
	m_ui.displayOnly->setChecked(m_settings.value("DisplayOnly", true).toBool());
	m_ui.leftThigh->setChecked(m_settings.value("LeftThigh", true). toBool());
	m_ui.leftLowerLeg->setChecked(m_settings.value("LeftLowerLeg", true). toBool());
	m_ui.rightThigh->setChecked(m_settings.value("RightThigh", true). toBool());
	m_ui.rightLowerLeg->setChecked(m_settings.value("RightLowerLeg", true). toBool());
}

void GraphPlotter::save()
{
	m_settings.setValue("SaveDir", m_ui.destDir->text());
	m_settings.setValue("BaseName", m_ui.baseFilename->text());
	m_settings.setValue("FileFormat", m_ui.fileFormat->currentIndex());
	m_settings.setValue("DisplayOnly", m_ui.displayOnly->isChecked());
	m_settings.setValue("LeftThigh", m_ui.leftThigh->isChecked());
	m_settings.setValue("LeftLowerLeg", m_ui.leftLowerLeg->isChecked());
	m_settings.setValue("RightThigh", m_ui.rightThigh->isChecked());
	m_settings.setValue("RightLowerLeg", m_ui.rightLowerLeg->isChecked());
}

void GraphPlotter::plot(const QString& templateFilename, int limb, const QString& outFilename, const QString& termType)
{
	QTemporaryFile data;
	data.open();
	QTextStream s(&data);
	
	switch (limb)
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
	commands.replace("__LIMB__", limbName(limb).toAscii());
	qDebug() << commands;
	
	data.close();
	
	QProcess gnuplot;
	gnuplot.setProcessChannelMode(QProcess::MergedChannels);
	
	gnuplot.start("gnuplot");
	gnuplot.write(commands);
	
	gnuplot.waitForFinished();
	
	qDebug() << gnuplot.readAll();
}

QTextStream& GraphPlotter::writeThighData(Part part, const Params<float>& initialParams, QTextStream& s)
{
	Params<int> p(0,
	              0,
	              ROUND(initialParams.lowerLegAlpha / ALPHA_STEP),
	              ROUND(initialParams.lowerLegTheta / THETA_STEP));
	
	for (p.thighAlpha=-ALPHA_RESOLUTION ; p.thighAlpha<=ALPHA_RESOLUTION ; p.thighAlpha++)
	{
		for (p.thighTheta=-THETA_RESOLUTION ; p.thighTheta<=THETA_RESOLUTION ; p.thighTheta++)
		{
			s << float(p.thighTheta) * THETA_STEP << " " << float(p.thighAlpha) * ALPHA_STEP << " " << m_info->result(p, part) << "\n";
		}
		s << "\n";
	}
	
	return s;
}

QTextStream& GraphPlotter::writeLowerLegData(Part part, const Params<float>& initialParams, QTextStream& s)
{
	Params<int> p(ROUND(initialParams.thighAlpha / ALPHA_STEP),
	              ROUND(initialParams.thighTheta / THETA_STEP),
	              0,
	              0);
	
	for (p.lowerLegAlpha=-ALPHA_RESOLUTION ; p.lowerLegAlpha<=ALPHA_RESOLUTION ; p.lowerLegAlpha++)
	{
		for (p.lowerLegTheta=-THETA_RESOLUTION ; p.lowerLegTheta<=THETA_RESOLUTION ; p.lowerLegTheta++)
		{
			s << float(p.lowerLegTheta) * THETA_STEP << " " << float(p.lowerLegAlpha) * ALPHA_STEP << " " << m_info->result(p, part) << "\n";
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

