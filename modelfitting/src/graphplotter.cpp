#include "graphplotter.h"
#include "listmodels.h"

#include <QTemporaryFile>
#include <QMessageBox>
#include <QProcess>
#include <QFileInfo>
#include <QFileDialog>

GraphPlotter::GraphPlotter(FrameModel* model, const QString& title, QWidget* parent)
	: QDialog(parent),
	  m_info(NULL),
	  m_tempFile(NULL),
	  m_model(model)
{
	m_ui.setupUi(this);
	setWindowTitle(title);
	
	m_ui.fftBox->hide();
	m_ui.dataSetBox->hide();
	
	m_filter = new FrameModelFilter(this);
	m_filter->setSourceModel(m_model);
	
	adjustSize();
	
	connect(m_ui.destDirBrowse, SIGNAL(clicked()), SLOT(browse()));
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
	aboutToShow();
	load();
	QDialog::exec();
	save();
}

void GraphPlotter::okClicked()
{
	switch (m_ui.fileFormat->currentIndex())
	{
		case 0: m_extension = ".png"; m_termType = "png";        break;
		case 1: m_extension = ".jpg"; m_termType = "jpeg";       break;
		case 2: m_extension = ".ps";  m_termType = "postscript"; break;
		case 3: m_extension = ".pdf"; m_termType = "pdf";        break;
		case 4: m_extension = ".tex"; m_termType = "latex";      break;
	}
	
	QString outFilename;
	if (!m_ui.displayOnly->isChecked())
	{
		if (!QFileInfo(m_ui.destDir->text()).isDir())
		{
			QMessageBox::warning(this, "Error", "\"" + m_ui.destDir->text() + "\" is not a directory");
			m_ui.destDir->setFocus(Qt::OtherFocusReason);
			return;
		}
		if (m_ui.baseFilename->text().isEmpty())
		{
			QMessageBox::warning(this, "Error", "Please enter a base filename");
			m_ui.baseFilename->setFocus(Qt::OtherFocusReason);
			return;
		}
		
		outFilename = m_ui.baseFilename->text();
	}
	
	plotData(outFilename);
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
	commands.replace("__OUT_FILENAME__", (m_ui.destDir->text() + QDir::separator() + filename + m_extension).toAscii());
	commands.replace("__TERM_TYPE__", m_termType.toAscii());
	//qDebug() << commands;
	
	if (m_ui.saveData->isChecked())
	{
		QString destFilename(m_ui.destDir->text() + QDir::separator() + filename + ".dat");
		if (QFile::exists(destFilename))
			if (QMessageBox::question(this, "File already exists", "The file \"" + destFilename + "\" alreay exists.  Overwrite?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
				QFile::remove(destFilename);
		m_tempFile->copy(destFilename); // This also closes the temp file
	}
	else
	{
		m_tempFile->close();
		
		QProcess gnuplot;
		gnuplot.setProcessChannelMode(QProcess::MergedChannels);
		
		gnuplot.start("gnuplot");
		gnuplot.write(commands);
		
		gnuplot.waitForFinished();
		
		qDebug() << gnuplot.readAll();
	}
	
	delete m_tempFile;
	m_tempFile = NULL;
}

void GraphPlotter::load()
{
	m_ui.destDir->setText(m_settings.value("SaveDir", QDir::homePath()).toString());
	m_ui.baseFilename->setText(m_settings.value("BaseName", "graph").toString());
	m_ui.fileFormat->setCurrentIndex(m_settings.value("FileFormat", 0).toInt());
	m_ui.displayOnly->setChecked(m_settings.value("DisplayOnly", true).toBool());
	
	m_ui.leftThigh->setChecked(m_settings.value("LeftThigh", true).toBool());
	m_ui.leftLowerLeg->setChecked(m_settings.value("LeftLowerLeg", true).toBool());
	m_ui.rightThigh->setChecked(m_settings.value("RightThigh", true).toBool());
	m_ui.rightLowerLeg->setChecked(m_settings.value("RightLowerLeg", true).toBool());
	
	m_ui.leftThighAlpha->setChecked(m_settings.value("LeftThighAlpha", true).toBool());
	m_ui.leftLowerAlpha->setChecked(m_settings.value("LeftLowerAlpha", true).toBool());
	m_ui.leftThighTheta->setChecked(m_settings.value("LeftThighTheta", true).toBool());
	m_ui.leftLowerTheta->setChecked(m_settings.value("LeftLowerTheta", true).toBool());
	
	m_ui.rightThighAlpha->setChecked(m_settings.value("RightThighAlpha", true).toBool());
	m_ui.rightLowerAlpha->setChecked(m_settings.value("RightLowerAlpha", true).toBool());
	m_ui.rightThighTheta->setChecked(m_settings.value("RightThighTheta", true).toBool());
	m_ui.rightLowerTheta->setChecked(m_settings.value("RightLowerTheta", true).toBool());
	
	m_ui.dataSetMin->setValue(m_settings.value("DataSetMin", m_ui.dataSetMin->minimum()).toInt());
	m_ui.dataSetMax->setValue(m_settings.value("DataSetMax", m_ui.dataSetMax->maximum()).toInt());
	
	m_ui.phase->setChecked(m_settings.value("Phase", false).toBool());
	m_ui.magnitude->setChecked(m_settings.value("Magnitude", false).toBool());
	m_ui.phaseMagnitude->setChecked(m_settings.value("PhaseMagnitude", true).toBool());
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
	
	m_settings.setValue("DataSetMin", m_ui.dataSetMin->value());
	m_settings.setValue("DataSetMax", m_ui.dataSetMax->value());
	
	m_settings.setValue("Phase", m_ui.phase->isChecked());
	m_settings.setValue("Magnitude", m_ui.magnitude->isChecked());
	m_settings.setValue("PhaseMagnitude", m_ui.phaseMagnitude->isChecked());
}

void GraphPlotter::browse()
{
	QString ret(QFileDialog::getExistingDirectory(this, "Browse", m_ui.destDir->text()));
	if (ret.isNull())
		return;
	
	m_ui.destDir->setText(ret);
}
