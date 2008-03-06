#include "errorcorrection.h"

ErrorCorrection::ErrorCorrection(QWidget* parent)
	: QDialog(parent)
{
	m_ui.setupUi(this);
	
	connect(m_ui.buttonBox, SIGNAL(accepted()), SLOT(okClicked()));
}

void ErrorCorrection::okClicked()
{
}



