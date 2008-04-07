#include "errorcorrection.h"
#include "frameset.h"
#include "frameinfo.h"
#include "curvefitter.h"

#include <cmath>

ErrorCorrection::ErrorCorrection(QWidget* parent)
	: QDialog(parent)
{
	m_ui.setupUi(this);
	connect(m_ui.buttonBox, SIGNAL(accepted()), SLOT(okClicked()));
	
	m_settings.beginGroup("ErrorCorrection");
	
	m_curveFitter = new CurveFitter;
}

ErrorCorrection::~ErrorCorrection()
{
	delete m_curveFitter;
}

void ErrorCorrection::setFrameSet(FrameSet* frameSet)
{
	m_curveFitter->setFrameSet(frameSet);
	
	int min = std::numeric_limits<int>::max();
	int max = std::numeric_limits<int>::min();
	
	for (int i=0 ; i<frameSet->count() ; ++i)
	{
		if (!frameSet->hasModelInformation(i))
			continue;
		min = qMin(min, i);
		max = qMax(max, i);
	}
	
	m_ui.tMin->setRange(min, max);
	m_ui.tMax->setRange(min, max);
	m_ui.tMin->setValue(m_settings.value("TMin", min).toInt());
	m_ui.tMax->setValue(m_settings.value("TMax", max).toInt());
}

void ErrorCorrection::okClicked()
{
	if (m_ui.tMax->value() < m_ui.tMax->value())
	{
		reject();
		return;
	}
	
	FittingResult result(m_curveFitter->doFitting(m_ui.tMin->value(), m_ui.tMax->value()));
	
	qDebug() << "Minimum:" << result;
	
	m_settings.setValue("TMin", m_ui.tMin->value());
	m_settings.setValue("TMax", m_ui.tMax->value());
	accept();
}

