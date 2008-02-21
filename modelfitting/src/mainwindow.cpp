#include "mainwindow.h"

#include <mesh.hh>
#include "frameinfo.h"
#include "imgprocessing.h"
#include "filter.h"
#include "convolution.h"
#include "meshfilter.h"

#include <QFileDialog>
#include <QTimer>
#include <QDebug>
#include <QTime>
#include <QProgressDialog>

MainWindow::MainWindow()
	: QDialog(NULL),
	  m_mesh(NULL),
	  m_voxelSpace(NULL),
	  m_frameInfo(NULL)
{
	m_ui.setupUi(this);
	
	connect(m_ui.openButton, SIGNAL(clicked()), SLOT(openDirectory()));
	connect(m_ui.recalculateButton, SIGNAL(clicked()), SLOT(recalculate()));
	connect(m_ui.recalculateAllButton, SIGNAL(clicked()), SLOT(recalculateAll()));
	
	connect(m_ui.fileList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), SLOT(loadSelectedFile()));
	
	m_thighFilter = new MeshFilter("thighmodel", ":meshsearch.lut");
	
	m_progressDialog = new QProgressDialog("Running map-reduce", 0, 0, 100, this);
	m_futureWatcher = new QFutureWatcher<ReduceType>(this);
	connect(m_futureWatcher, SIGNAL(started()), m_progressDialog, SLOT(show()));
	connect(m_futureWatcher, SIGNAL(finished()), m_progressDialog, SLOT(hide()));
	connect(m_futureWatcher, SIGNAL(progressRangeChanged(int, int)), m_progressDialog, SLOT(setRange(int, int)));
	connect(m_futureWatcher, SIGNAL(progressValueChanged(int)), m_progressDialog, SLOT(setValue(int)));
	
	m_ui.front->setViewType(GLView::Front);
	m_ui.side->setViewType(GLView::Side);
	m_ui.overhead->setViewType(GLView::Overhead);
	m_ui.angle->setViewType(GLView::Angle);
	
	m_ui.front->setThighFilter(m_thighFilter);
	m_ui.side->setThighFilter(m_thighFilter);
	m_ui.overhead->setThighFilter(m_thighFilter);
	m_ui.angle->setThighFilter(m_thighFilter);
	
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
	
	QList<int> sizes;
	sizes.append(width());
	sizes.append(100);
	m_ui.splitter->setSizes(sizes);
	
	m_openDir = m_settings.value("OpenDir", QDir::homePath()).toString();
	
	updateFileListing();
}

MainWindow::~MainWindow()
{
	clearMesh();
	delete m_thighFilter;
}

void MainWindow::clearMesh()
{
	if (m_mesh)
		m_mesh->draw_destroy();
	
	delete m_mesh;
	delete m_voxelSpace;
	delete m_frameInfo;
	
	m_mesh = NULL;
	m_voxelSpace = NULL;
	m_frameInfo = NULL;
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
	
	clearMesh();
	m_ui.fileList->clear();
	m_ui.fileList->addItems(fileNames);
	
	if (m_ui.fileList->count() > 0)
		m_ui.fileList->setCurrentRow(0);
	
	m_ui.recalculateButton->setEnabled(m_ui.fileList->count() > 0);
	m_ui.recalculateAllButton->setEnabled(m_ui.fileList->count() > 0);
}

void MainWindow::loadSelectedFile()
{
	if (m_ui.fileList->currentItem() == NULL)
		return;
	
	QString fileName = m_openDir + QDir::separator() + m_ui.fileList->currentItem()->text();
	
	clearMesh();
	m_voxelSpace = new Voxel_Space(fileName.toAscii().data());
	m_mesh = new Mesh(*m_voxelSpace);
	m_mesh->draw_init(true);
	
	m_ui.front->setMesh(m_mesh);
	m_ui.side->setMesh(m_mesh);
	m_ui.overhead->setMesh(m_mesh);
	m_ui.angle->setMesh(m_mesh);
}

void MainWindow::updateViews()
{
	m_ui.front->updateGL();
	m_ui.side->updateGL();
	m_ui.overhead->updateGL();
	m_ui.angle->updateGL();
	
	m_redrawTimer->start(30);
}

void MainWindow::recalculate()
{
	delete m_frameInfo;
	m_frameInfo = new FrameInfo(m_voxelSpace);
	m_frameInfo->analyse();
	
	m_ui.front->setFrameInfo(m_frameInfo);
	m_ui.side->setFrameInfo(m_frameInfo);
	m_ui.overhead->setFrameInfo(m_frameInfo);
	m_ui.angle->setFrameInfo(m_frameInfo);
	
	QFuture<ReduceType> future = m_thighFilter->correlate(m_frameInfo);
	m_futureWatcher->setFuture(future);
	m_progressDialog->exec(); // Doesn't return until the mapreduce is done
	
	m_frameInfo->setFeatureVec(future.result().first);
}

void MainWindow::recalculateAll()
{
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

