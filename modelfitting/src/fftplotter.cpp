#include "fftplotter.h"
#include "frameset.h"

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
	m_dataSize = m_ui.dataSetMax->value() - m_ui.dataSetMin->value();
	if (m_dataSize <= 0)
	{
		QMessageBox::warning(this, "Invalid data set range", "The maximum must be greater than the minimum");
		return;
	}
	
	m_data = (double*) fftw_malloc(sizeof(double) * m_dataSize);
	m_results = (std::complex<double>*) fftw_malloc(sizeof(std::complex<double>) * (m_dataSize/2 + 1));
	m_plan = fftw_plan_dft_r2c_1d(m_dataSize, m_data, reinterpret_cast<fftw_complex*>(m_results), FFTW_ESTIMATE | FFTW_DESTROY_INPUT);
	
	runAndPlot(LeftThighTheta, outFilename + "-leftthightheta");
	
	fftw_destroy_plan(m_plan);
	fftw_free(m_data);
	fftw_free(m_results);
}

void FftPlotter::initData(Type type)
{
	const double* dataEnd = m_data + m_dataSize;
	double* p = m_data;
	
	int i = m_ui.dataSetMin->value();
	while (p != dataEnd)
	{
		if (!frameInfo()->frameSet()->hasModelInformation(i))
			*p = 0.0;
		else
		{
			FrameInfo* info = frameInfo()->frameSet()->loadFrame(i, true);
			
			switch (type)
			{
				case LeftThighAlpha:  *p = info->leftLeg().thighAlpha;     break;
				case LeftThighTheta:  *p = info->leftLeg().thighTheta;     break;
				case LeftLowerAlpha:  *p = info->leftLeg().lowerLegAlpha;  break;
				case LeftLowerTheta:  *p = info->leftLeg().lowerLegTheta;  break;
				case RightThighAlpha: *p = info->rightLeg().thighAlpha;    break;
				case RightThighTheta: *p = info->rightLeg().thighTheta;    break;
				case RightLowerAlpha: *p = info->rightLeg().lowerLegAlpha; break;
				case RightLowerTheta: *p = info->rightLeg().lowerLegTheta; break;
			}
			
			delete info;
		}
		
		i++;
		p++;
	}
}

void FftPlotter::runAndPlot(Type type, const QString& outFilename)
{
	initData(type);
	fftw_execute(m_plan);
	
	QTextStream& s = openTempFile();
	
	for (int i=0 ; i<m_dataSize/2+1 ; ++i)
	{
		double magnitude = std::abs(m_results[i]);
		double phase = std::arg(m_results[i]);
		
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
	QStringList plots;
	
	if (m_ui.magnitude->isChecked()) plots << "'__DATA_FILENAME__' using 1:2 title \"Magnitude\" with impulses lt 1";
	if (m_ui.phase->isChecked()) plots << "'__DATA_FILENAME__' using 1:3 title \"Phase\" with impulses lt 3";
	if (m_ui.phaseMagnitude->isChecked()) plots << "'__DATA_FILENAME__' using 1:4 title \"Phase * Magnitude\" with impulses lt 4";
	
	commands.replace("__PLOTS__", plots.join(", ").toAscii());
}
