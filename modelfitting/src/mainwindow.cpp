#include "mainwindow.h"

#include <mesh.hh>
#include "frameinfo.h"
#include "mapreduceprogress.h"
#include "graphplotter.h"

#include <QFileDialog>
#include <QTimer>
#include <QDebug>
#include <QTime>
#include <QMessageBox>

MainWindow::MainWindow()
	: QMainWindow(NULL),
	  m_mesh(NULL),
	  m_voxelSpace(NULL),
	  m_frameInfo(NULL),
	  m_paramUpdatesDisabled(false)
{
	m_ui.setupUi(this);
	
	connect(m_ui.actionOpenDirectory, SIGNAL(triggered(bool)), SLOT(openDirectory()));
	connect(m_ui.actionRecalculate, SIGNAL(triggered(bool)), SLOT(recalculate()));
	connect(m_ui.actionRecalculateAll, SIGNAL(triggered(bool)), SLOT(recalculateAll()));
	
	connect(m_ui.fileList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), SLOT(loadSelectedFile()));
	
	m_progressDialog = new MapReduceProgress(this);
	
	m_graphPlotter = new GraphPlotter(this);
	connect(m_ui.actionPlotEnergyGraphs, SIGNAL(triggered(bool)), m_graphPlotter, SLOT(exec()));
	
	m_ui.front->setViewType(GLView::Front);
	m_ui.side->setViewType(GLView::Side);
	m_ui.overhead->setViewType(GLView::Overhead);
	m_ui.angle->setViewType(GLView::Angle);
	
	m_redrawTimer = new QTimer(this);
	m_redrawTimer->setSingleShot(true);
	connect(m_redrawTimer, SIGNAL(timeout()), SLOT(updateViews()));
	m_redrawTimer->start(0);
	
	m_ui.crossFrontSlider->setMaximum(m_ui.front->extent());
	m_ui.crossSideSlider->setMaximum(m_ui.side->extent());
	m_ui.crossTopSlider->setMaximum(m_ui.overhead->extent());
	
	connect(m_ui.crossFrontSlider, SIGNAL(sliderMoved(int)), SLOT(sliderMoved(int)));
	connect(m_ui.crossSideSlider, SIGNAL(sliderMoved(int)), SLOT(sliderMoved(int)));
	connect(m_ui.crossTopSlider, SIGNAL(sliderMoved(int)), SLOT(sliderMoved(int)));
	
	connect(m_ui.actionShowModel, SIGNAL(toggled(bool)), m_ui.front, SLOT(setModelVisible(bool)));
	connect(m_ui.actionShowModel, SIGNAL(toggled(bool)), m_ui.side, SLOT(setModelVisible(bool)));
	connect(m_ui.actionShowModel, SIGNAL(toggled(bool)), m_ui.overhead, SLOT(setModelVisible(bool)));
	connect(m_ui.actionShowModel, SIGNAL(toggled(bool)), m_ui.angle, SLOT(setModelVisible(bool)));
	
	connect(m_ui.actionShowVoxelData, SIGNAL(toggled(bool)), m_ui.front, SLOT(setVoxelDataVisible(bool)));
	connect(m_ui.actionShowVoxelData, SIGNAL(toggled(bool)), m_ui.side, SLOT(setVoxelDataVisible(bool)));
	connect(m_ui.actionShowVoxelData, SIGNAL(toggled(bool)), m_ui.overhead, SLOT(setVoxelDataVisible(bool)));
	connect(m_ui.actionShowVoxelData, SIGNAL(toggled(bool)), m_ui.angle, SLOT(setVoxelDataVisible(bool)));
	
	connect(m_ui.actionAboutQt, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(aboutQt()));
	connect(m_ui.actionQuit, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));
	
	setupSpinBox(m_ui.leftThighAlpha, ALPHA_RANGE, ALPHA_STEP);
	setupSpinBox(m_ui.leftThighTheta, THETA_RANGE, THETA_STEP);
	setupSpinBox(m_ui.leftLowerAlpha, ALPHA_RANGE, ALPHA_STEP);
	setupSpinBox(m_ui.leftLowerTheta, THETA_RANGE, THETA_STEP);
	setupSpinBox(m_ui.rightThighAlpha, ALPHA_RANGE, ALPHA_STEP);
	setupSpinBox(m_ui.rightThighTheta, THETA_RANGE, THETA_STEP);
	setupSpinBox(m_ui.rightLowerAlpha, ALPHA_RANGE, ALPHA_STEP);
	setupSpinBox(m_ui.rightLowerTheta, THETA_RANGE, THETA_STEP);
	
	m_openDir = m_settings.value("OpenDir", QDir::homePath()).toString();
	
	// We do this in a single shot timer so that initalizeGL() gets called on the GLViews before creating any
	// FrameInfo objects.  FrameInfo objects call Mesh::draw_init() which needs to have had
	// setupWinGLFunctions() run beforehand
	QTimer::singleShot(0, this, SLOT(updateFileListing()));
}

MainWindow::~MainWindow()
{
	delete m_frameInfo;
}

void MainWindow::setupSpinBox(QDoubleSpinBox* spinner, double range, double step)
{
	connect(spinner, SIGNAL(valueChanged(double)), SLOT(setInfoParams()));
	spinner->setDecimals(5);
	spinner->setMinimum(-range);
	spinner->setMaximum(range);
	spinner->setSingleStep(step);
	spinner->setValue(0.0);
}

void MainWindow::openDirectory()
{
	QString dirName = QFileDialog::getExistingDirectory(this, "Open directory", m_openDir);
	
	if (dirName.isNull())
		return;
	
	m_openDir = dirName;
	m_settings.setValue("OpenDir", m_openDir);
	
	updateFileListing();
}

void MainWindow::updateFileListing()
{
	if (m_openDir.isNull())
		return;
	
	QDir dir(m_openDir);
	QStringList fileNames(dir.entryList(QStringList() << "*.Zspc", QDir::Files | QDir::NoDotAndDotDot | QDir::Readable, QDir::Name));
	
	m_ui.fileList->clear();
	m_ui.fileList->addItems(fileNames);
	
	if (m_ui.fileList->count() > 0)
		m_ui.fileList->setCurrentRow(0);
	
	m_ui.actionRecalculate->setEnabled(m_ui.fileList->count() > 0);
	m_ui.actionRecalculateAll->setEnabled(m_ui.fileList->count() > 0);
}

void MainWindow::loadSelectedFile()
{
	if (m_ui.fileList->currentItem() == NULL)
		return;
	
	QString fileName = m_openDir + QDir::separator() + m_ui.fileList->currentItem()->text();
	
	delete m_frameInfo;
	m_frameInfo = new FrameInfo(fileName);
	
	m_ui.actionPlotEnergyGraphs->setEnabled(false);
	m_ui.leftLegGroup->setEnabled(m_frameInfo->hasModelInformation());
	m_ui.rightLegGroup->setEnabled(m_frameInfo->hasModelInformation());
	
	m_ui.front->setFrameInfo(m_frameInfo);
	m_ui.side->setFrameInfo(m_frameInfo);
	m_ui.overhead->setFrameInfo(m_frameInfo);
	m_ui.angle->setFrameInfo(m_frameInfo);
	
	m_graphPlotter->setFrameInfo(m_frameInfo);
	
	getInfoParams();
}

void MainWindow::updateViews()
{
	m_ui.front->updateGL();
	m_ui.side->updateGL();
	m_ui.overhead->updateGL();
	m_ui.angle->updateGL();
	
	m_redrawTimer->start(30);
}

bool MainWindow::recalculate()
{
	m_progressDialog->addOperations(m_frameInfo->update());
	int ret = m_progressDialog->exec("Fitting model to " + QFileInfo(m_frameInfo->filename()).fileName());
	
	getInfoParams();
	
	m_ui.actionPlotEnergyGraphs->setEnabled(m_frameInfo->hasModelInformation());
	m_ui.leftLegGroup->setEnabled(m_frameInfo->hasModelInformation());
	m_ui.rightLegGroup->setEnabled(m_frameInfo->hasModelInformation());
	
	return ret == QDialog::Accepted;
}

void MainWindow::recalculateAll()
{
	for (int i=0 ; i<m_ui.fileList->count() ; i++)
	{
		m_ui.fileList->setCurrentRow(i);
		if (!recalculate())
			break;
	}
}

void MainWindow::initializeGL()
{
}

void MainWindow::sliderMoved(int value)
{
	GLView* view;
	QLabel* label;
	
	if (sender() == m_ui.crossFrontSlider)
	{
		view = m_ui.front;
		label = m_ui.crossFrontLabel;
	}
	else if (sender() == m_ui.crossSideSlider)
	{
		view = m_ui.side;
		label = m_ui.crossSideLabel;
	}
	else if (sender() == m_ui.crossTopSlider)
	{
		view = m_ui.overhead;
		label = m_ui.crossTopLabel;
	}
	else
		return;
	
	if (value == -1)
		label->setText("Off");
	else
		label->setText(QString::number(value));
	
	view->setCrossSection(value);
}

void MainWindow::setInfoParams()
{
	if (!m_frameInfo || m_paramUpdatesDisabled)
		return;
	
	Params<float> left(m_ui.leftThighAlpha->value(),
	                   m_ui.leftThighTheta->value(),
	                   m_ui.leftLowerAlpha->value(),
	                   m_ui.leftLowerTheta->value());
	Params<float> right(m_ui.rightThighAlpha->value(),
	                    m_ui.rightThighTheta->value(),
	                    m_ui.rightLowerAlpha->value(),
	                    m_ui.rightLowerTheta->value());
	
	m_frameInfo->setLeftLeg(left);
	m_frameInfo->setRightLeg(right);
}

void MainWindow::getInfoParams()
{
	m_paramUpdatesDisabled = true;
	m_ui.leftThighAlpha->setValue(m_frameInfo->leftLeg().thighAlpha);
	m_ui.leftThighTheta->setValue(m_frameInfo->leftLeg().thighTheta);
	m_ui.leftLowerAlpha->setValue(m_frameInfo->leftLeg().lowerLegAlpha);
	m_ui.leftLowerTheta->setValue(m_frameInfo->leftLeg().lowerLegTheta);
	
	m_ui.rightThighAlpha->setValue(m_frameInfo->rightLeg().thighAlpha);
	m_ui.rightThighTheta->setValue(m_frameInfo->rightLeg().thighTheta);
	m_ui.rightLowerAlpha->setValue(m_frameInfo->rightLeg().lowerLegAlpha);
	m_ui.rightLowerTheta->setValue(m_frameInfo->rightLeg().lowerLegTheta);
	m_paramUpdatesDisabled = false;
}
