#include "controller.h"
#include "engine.h"
#include "gaussian.h"
#include "intensitygradient.h"

#include <QFileDialog>
#include <QDir>
#include <QCoreApplication>

Controller::Controller(Engine* engine)
	: QDialog(),
	  m_engine(engine)
{
	m_ui.setupUi(this);

	connect(m_ui.openFileButton, SIGNAL(clicked()), SLOT(openFile()));
	connect(m_ui.quitButton, SIGNAL(clicked()), QCoreApplication::instance(), SLOT(quit()));
}

Controller::~Controller()
{
}

void Controller::openFile()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Open image", QDir::homePath(), "Images (*.png *.jpg *.jpeg *.bmp *.mng *.gif *.pbm *.pgm *.ppm *.xbm *.xpm");

	m_engine->setImage(fileName);
	m_engine->setFilterSet(new Gaussian());
}


