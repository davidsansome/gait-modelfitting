#include "mainwindow.h"

#include <mesh.hh>
#include "frameinfo.h"
#include "mapreduceprogress.h"
#include "energyplotter.h"
#include "timeplotter.h"
#include "errorcorrection.h"
#include "fftplotter.h"
#include "classifydialog.h"
#include "listmodels.h"
#include "people.h"

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
	
	m_model = new FrameModel(this);
	m_frameSetFilterModel = new FrameSetFilterModel(this);
	m_frameSetFilterModel->setSourceModel(m_model);
	m_frameFilterModel = new FrameModelFilter(this);
	m_frameFilterModel->setSourceModel(m_model);
	
	m_ui.frameSetView->setModel(m_frameSetFilterModel);
	m_ui.frameView->setModel(m_frameFilterModel);
	
	People::setModel(m_model);
	
	connect(m_ui.actionOpenDirectory, SIGNAL(triggered(bool)), SLOT(openDirectory()));
	connect(m_ui.actionRecalculate, SIGNAL(triggered(bool)), SLOT(recalculate()));
	connect(m_ui.actionRecalculateAll, SIGNAL(triggered(bool)), SLOT(recalculateAll()));
	
	connect(m_ui.frameSetView->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), SLOT(frameSetActivated(const QModelIndex&)));
	connect(m_ui.frameView->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)), SLOT(frameActivated(const QModelIndex&)));
	
	m_progressDialog = new MapReduceProgress(this);
	
	//m_energyPlotter = new EnergyPlotter(m_model, this);
	//connect(m_ui.actionPlotEnergyGraphs, SIGNAL(triggered(bool)), m_energyPlotter, SLOT(exec()));
	
	m_timePlotter = new TimePlotter(m_model, this);
	connect(m_ui.actionPlotTimeGraphs, SIGNAL(triggered(bool)), m_timePlotter, SLOT(exec()));
	
	m_fftPlotter = new FftPlotter(m_model, this);
	connect(m_ui.actionPlotFftGraphs, SIGNAL(triggered(bool)), m_fftPlotter, SLOT(exec()));
	
	m_errorCorrection = new ErrorCorrection(m_model, this);
	connect(m_ui.actionErrorCorrection, SIGNAL(triggered(bool)), m_errorCorrection, SLOT(exec()));
	
	m_classifyDialog = new ClassifyDialog(m_model, this);
	connect(m_ui.actionClassify, SIGNAL(triggered(bool)), SLOT(classify()));
	
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
	
	connect(m_ui.actionOrthogonalViews, SIGNAL(toggled(bool)), SLOT(orthogonalToggled(bool)));
	
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
	
	QSettings settings;
	m_openDir = settings.value("OpenDir", QDir::homePath()).toString();
	m_ui.actionOrthogonalViews->setChecked(settings.value("OrthogonalViews", true).toBool());
	
	// We do this in a single shot timer so that initalizeGL() gets called on the GLViews before creating any
	// FrameInfo objects.  FrameInfo objects call Mesh::draw_init() which needs to have had
	// setupWinGLFunctions() run beforehand
	QTimer::singleShot(0, this, SLOT(openDirChanged()));
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
	QSettings settings;
	settings.setValue("OpenDir", m_openDir);
	
	openDirChanged();
}

void MainWindow::openDirChanged()
{
	QModelIndex rootNode(m_frameSetFilterModel->mapFromSource(m_model->index(m_openDir)));
	m_ui.frameSetView->setRootIndex(rootNode);
	m_frameSetFilterModel->setRootIndex(rootNode);
}

void MainWindow::setFrameInfo(FrameInfo* frameInfo)
{
	delete m_frameInfo;
	m_frameInfo = frameInfo;
	
	m_ui.actionPlotEnergyGraphs->setEnabled(false);
	m_ui.leftLegGroup->setEnabled(m_frameInfo && m_frameInfo->hasModelInformation());
	m_ui.rightLegGroup->setEnabled(m_frameInfo && m_frameInfo->hasModelInformation());
	
	m_ui.front->setFrameInfo(m_frameInfo);
	m_ui.side->setFrameInfo(m_frameInfo);
	m_ui.overhead->setFrameInfo(m_frameInfo);
	m_ui.angle->setFrameInfo(m_frameInfo);
	
	//m_energyPlotter->setFrameInfo(m_frameInfo);
	m_timePlotter->setFrameInfo(m_frameInfo);
	m_fftPlotter->setFrameInfo(m_frameInfo);
	
	if (m_frameInfo)
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
	int ret = m_progressDialog->exec("Fitting model to " + QFileInfo(m_frameInfo->fileName()).fileName());
	
	getInfoParams();
	
	m_ui.actionPlotEnergyGraphs->setEnabled(m_frameInfo->hasModelInformation());
	m_ui.leftLegGroup->setEnabled(m_frameInfo->hasModelInformation());
	m_ui.rightLegGroup->setEnabled(m_frameInfo->hasModelInformation());
	
	if (m_frameInfo->hasModelInformation())
	{
		m_ui.actionPlotTimeGraphs->setEnabled(true);
		m_ui.actionPlotFftGraphs->setEnabled(true);
		m_ui.actionClassify->setEnabled(true);
		m_ui.actionErrorCorrection->setEnabled(true);
	}
	
	return ret == QDialog::Accepted;
}

void MainWindow::recalculateAll()
{
	QModelIndex root(m_frameFilterModel->mapFromSource(m_frameInfo->index().parent()));
	int count = m_frameFilterModel->rowCount(root);
	for (int i=0 ; i<count ; ++i)
	{
		m_ui.frameView->selectionModel()->setCurrentIndex(root.child(i, 0), QItemSelectionModel::ClearAndSelect);
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

void MainWindow::classify()
{
	m_model->updateSignature(m_frameInfo->index().parent());
	
	m_classifyDialog->setFrameSet(m_frameInfo->index().parent());
	m_classifyDialog->exec();
}

void MainWindow::frameSetActivated(const QModelIndex& index)
{
	QModelIndex realIndex(index.sibling(index.row(), 0));
	QModelIndex dirSource(m_frameSetFilterModel->mapToSource(realIndex));
	
	m_frameFilterModel->setRootIndex(dirSource);
	m_ui.frameView->setRootIndex(m_frameFilterModel->mapFromSource(dirSource));
	
	QModelIndex dirFrames(m_frameFilterModel->mapFromSource(dirSource));
	int childFrames = m_frameFilterModel->rowCount(dirFrames);
	
	bool someFramesHaveInfo = false;
	for (int i=0 ; i<childFrames ; ++i)
	{
		if (m_model->hasModelInformation(m_frameFilterModel->mapToSource(dirFrames.child(i, 0))))
			someFramesHaveInfo = true;
	}
	
	m_ui.actionRecalculate->setEnabled(childFrames > 0);
	m_ui.actionRecalculateAll->setEnabled(childFrames > 0);
	
	m_ui.actionPlotTimeGraphs->setEnabled(someFramesHaveInfo);
	m_ui.actionPlotFftGraphs->setEnabled(someFramesHaveInfo);
	m_ui.actionClassify->setEnabled(someFramesHaveInfo);
	m_ui.actionErrorCorrection->setEnabled(someFramesHaveInfo);
	
	m_errorCorrection->setFrameSet(dirSource);
	
	if (childFrames > 0)
		m_ui.frameView->selectionModel()->setCurrentIndex(dirFrames.child(0, 0), QItemSelectionModel::ClearAndSelect);
}

void MainWindow::frameActivated(const QModelIndex& index)
{
	FrameInfo* newFrameInfo = m_model->loadFrame(m_frameFilterModel->mapToSource(index));
	if (newFrameInfo == NULL)
	{
		QMessageBox::warning(this, "Error", "Could not load the frame " + index.data(Qt::DisplayRole).toString() + ".  Perhaps the file no longer exists?");
		return;
	}
	
	setFrameInfo(newFrameInfo);
}

void MainWindow::orthogonalToggled(bool value)
{
	m_ui.front->setVisible(value);
	m_ui.side->setVisible(value);
	m_ui.overhead->setVisible(value);
	
	QSettings settings;
	settings.setValue("OrthogonalViews", value);
}

