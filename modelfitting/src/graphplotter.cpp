#include "graphplotter.h"

#include <QTemporaryFile>
#include <QMessageBox>
#include <QProcess>
#include <QFileInfo>
#include <QFileDialog>

GraphPlotter::GraphPlotter(QWidget* parent)
	: QDialog(parent),
	  m_info(NULL),
	  m_tempFile(NULL)
{
	m_ui.setupUi(this);
	
	connect(m_ui.buttonBox, SIGNAL(accepted()), SLOT(okClicked()));
	
	m_settings.beginGroup("GraphPlotter");
}

GraphPlotter::~GraphPlotter()
{
	delete m_tempFile;
	m_tempFile = NULL;
}

void GraphPlotter::exec()
{
	load();
	QDialog::exec();
	save();
}

void GraphPlotter::okClicked()
{
	QString extension;
	switch (m_ui.fileFormat->currentIndex())
	{
		case 0: extension = ".png"; m_termType = "png";        break;
		case 1: extension = ".jpg"; m_termType = "jpeg";       break;
		case 2: extension = ".ps";  m_termType = "postscript"; break;
		case 3: extension = ".pdf"; m_termType = "pdf";        break;
	}
	
	QString outFilename;
	if (!m_ui.displayOnly->isChecked())
	{
		if (!QFileInfo(m_ui.destDir->text()).isDir())
		{
			QMessageBox::warning(this, "Error", "\"" + m_ui.destDir->text() + "\" is not a directory");
			return;
		}
		
		outFilename = m_ui.destDir->text() + QDir::separator() + m_ui.baseFilename->text();
	}
	
	plotData(outFilename, extension);
	accept();
}

QTextStream& GraphPlotter::openTempFile()
{
	delete m_tempFile;
	m_tempFile = new QTemporaryFile();
	m_tempFile->open();
	m_tempFileName = m_tempFile->fileName();
	
	m_stream.setDevice(m_tempFile);
	return m_stream;
}

void GraphPlotter::saveGraph(const QString& filename)
{
	QFile templateFile(templateName(m_ui.displayOnly->isChecked()));
	templateFile.open(QIODevice::ReadOnly);
	QByteArray commands(templateFile.readAll());
	
	replaceTokens(commands);
	commands.replace("__DATA_FILENAME__", m_tempFileName.toAscii());
	commands.replace("__OUT_FILENAME__", filename.toAscii());
	commands.replace("__TERM_TYPE__", m_termType.toAscii());
	qDebug() << commands;
	
	m_tempFile->close();
	
	QProcess gnuplot;
	gnuplot.setProcessChannelMode(QProcess::MergedChannels);
	
	gnuplot.start("gnuplot");
	gnuplot.write(commands);
	
	gnuplot.waitForFinished();
	
	qDebug() << gnuplot.readAll();
	
	delete m_tempFile;
	m_tempFile = NULL;
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
	
	m_ui.leftThighAlpha->setChecked(m_settings.value("LeftThighAlpha", true). toBool());
	m_ui.leftLowerAlpha->setChecked(m_settings.value("LeftLowerAlpha", true). toBool());
	m_ui.leftThighTheta->setChecked(m_settings.value("LeftThighTheta", true). toBool());
	m_ui.leftLowerTheta->setChecked(m_settings.value("LeftLowerTheta", true). toBool());
	
	m_ui.rightThighAlpha->setChecked(m_settings.value("RightThighAlpha", true). toBool());
	m_ui.rightLowerAlpha->setChecked(m_settings.value("RightLowerAlpha", true). toBool());
	m_ui.rightThighTheta->setChecked(m_settings.value("RightThighTheta", true). toBool());
	m_ui.rightLowerTheta->setChecked(m_settings.value("RightLowerTheta", true). toBool());
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
	
	m_settings.setValue("LeftThighAlpha", m_ui.leftThighAlpha->isChecked());
	m_settings.setValue("LeftLowerAlpha", m_ui.leftLowerAlpha->isChecked());
	m_settings.setValue("LeftThighTheta", m_ui.leftThighTheta->isChecked());
	m_settings.setValue("LeftLowerTheta", m_ui.leftLowerTheta->isChecked());
	
	m_settings.setValue("RightThighAlpha", m_ui.rightThighAlpha->isChecked());
	m_settings.setValue("RightLowerAlpha", m_ui.rightLowerAlpha->isChecked());
	m_settings.setValue("RightThighTheta", m_ui.rightThighTheta->isChecked());
	m_settings.setValue("RightLowerTheta", m_ui.rightLowerTheta->isChecked());
}
