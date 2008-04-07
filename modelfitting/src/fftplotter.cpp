#include "fftplotter.h"
#include "frameset.h"
#include "curvefitter.h"

#include <QFileInfo>
#include <QDir>
#include <QMessageBox>

#include <fftw3.h>

FftPlotter::FftPlotter(QWidget* parent)
	: GraphPlotter("FFT graph plotter", parent)
{
	m_ui.stackedWidget->setCurrentWidget(m_ui.timePage);
	m_ui.dataSetBox->show();
	m_ui.fftBox->show();
	adjustSize();
	
	m_fft = new Fft;
}

FftPlotter::~FftPlotter()
{
	delete m_fft;
}

void FftPlotter::aboutToShow()
{
	int min = std::numeric_limits<int>::max();
	int max = std::numeric_limits<int>::min();
	
	for (int i=0 ; i<frameInfo()->frameSet()->count() ; ++i)
	{
		if (!frameInfo()->frameSet()->hasModelInformation(i))
			continue;
		min = qMin(min, i);
		max = qMax(max, i);
	}
	
	m_ui.dataSetMin->setRange(min, max);
	m_ui.dataSetMax->setRange(min, max);
}

QString FftPlotter::templateName(bool displayOnly) const
{
	if (displayOnly)
		return ":showfft.g";
	return ":savefft.g";
}

void FftPlotter::plotData(const QString& outFilename)
{
	m_fft->setFrameSet(frameInfo()->frameSet());
	if (m_ui.dataSetAuto->isChecked())
		m_fft->init();
	else
	{
		if (m_ui.dataSetMax->value() - m_ui.dataSetMin->value() <= 0)
		{
			QMessageBox::warning(this, "Invalid data set range", "The maximum must be greater than the minimum");
			return;
		}
		m_fft->init(m_ui.dataSetMin->value(), m_ui.dataSetMax->value());
	}
	
	if (m_ui.leftThighAlpha->isChecked())  runAndPlot(Fft::LeftThighAlpha,  outFilename + "-leftthighalpha");
	if (m_ui.leftThighTheta->isChecked())  runAndPlot(Fft::LeftThighTheta,  outFilename + "-leftthightheta");
	if (m_ui.leftLowerAlpha->isChecked())  runAndPlot(Fft::LeftLowerAlpha,  outFilename + "-leftloweralpha");
	if (m_ui.leftLowerTheta->isChecked())  runAndPlot(Fft::LeftLowerTheta,  outFilename + "-leftlowertheta");
	if (m_ui.rightThighAlpha->isChecked()) runAndPlot(Fft::RightThighAlpha, outFilename + "-rightthighalpha");
	if (m_ui.rightThighTheta->isChecked()) runAndPlot(Fft::RightThighTheta, outFilename + "-rightthightheta");
	if (m_ui.rightLowerAlpha->isChecked()) runAndPlot(Fft::RightLowerAlpha, outFilename + "-rightloweralpha");
	if (m_ui.rightLowerTheta->isChecked()) runAndPlot(Fft::RightLowerTheta, outFilename + "-rightlowertheta");
}

void FftPlotter::runAndPlot(Fft::Type type, const QString& outFilename)
{
	m_fft->run(type);
	
	if (m_ui.magnitude->isChecked())      plot("'__DATA_FILENAME__' using 1:2 title \"Magnitude\" with impulses lt 1",         outFilename + "-magnitude");
	if (m_ui.phase->isChecked())          plot("'__DATA_FILENAME__' using 1:3 title \"Phase\" with impulses lt 3",             outFilename + "-phase");
	if (m_ui.phaseMagnitude->isChecked()) plot("'__DATA_FILENAME__' using 1:4 title \"Phase * Magnitude\" with impulses lt 4", outFilename + "-phasemagnitude");
}

void FftPlotter::plot(const QString& plot, const QString& outFilename)
{
	m_plot = plot;
	
	QTextStream& s = openTempFile();
	
	for (int i=0 ; i<m_fft->resultSize() ; ++i)
	{
		double magnitude = std::abs(m_fft->result()[i]);
		double phase = std::arg(m_fft->result()[i]);
		
		QStringList cols;
		cols << QString::number(i);
		cols << QString::number(magnitude, 'f');
		cols << QString::number(phase, 'f');
		cols << QString::number(phase * magnitude, 'f');
		
		s << cols.join(" ") << "\n";
	}
	
	saveGraph(outFilename);
}

void FftPlotter::replaceTokens(QByteArray& commands)
{
	commands.replace("__PLOTS__", m_plot.toAscii());
}
