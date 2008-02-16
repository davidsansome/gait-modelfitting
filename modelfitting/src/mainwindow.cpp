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

MainWindow::MainWindow()
	: QDialog(NULL),
	  m_mesh(NULL),
	  m_voxelSpace(NULL),
	  m_frameInfo(NULL)
{
	m_ui.setupUi(this);
	
	connect(m_ui.openButton, SIGNAL(clicked()), SLOT(openMesh()));
	connect(m_ui.findCenterButton, SIGNAL(clicked()), SLOT(findCenter()));
	
	m_thighFilter = new MeshFilter("thighmodel", ":meshsearch.lut");
	
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
	
	connect(m_ui.imgProcessing, SIGNAL(setupReady()), SLOT(initializeGL()));
	
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

void MainWindow::openMesh()
{
	QString fileName = QFileDialog::getOpenFileName(this, "Open file", m_openDir, "*.Zspc");
	
	if (fileName.isNull())
		return;
	
	QFileInfo fileInfo(fileName);
	m_ui.meshName->setText(fileInfo.baseName());
	m_openDir = fileInfo.path();
	m_settings.setValue("OpenDir", m_openDir);
	
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

void MainWindow::findCenter()
{
	delete m_frameInfo;
	m_frameInfo = new FrameInfo(m_voxelSpace);
	m_frameInfo->analyse();
	
	m_ui.front->setFrameInfo(m_frameInfo);
	m_ui.side->setFrameInfo(m_frameInfo);
	m_ui.overhead->setFrameInfo(m_frameInfo);
	m_ui.angle->setFrameInfo(m_frameInfo);
	
	QTime t;
	t.start();
	
	/*Filter* thigh = new Filter("filters/thigh.filter");
	Convolution* convolution = new Convolution(thigh, m_frameInfo);
	m_ui.imgProcessing->setFilterSet(convolution);
	m_ui.imgProcessing->updateGL();
	m_frameInfo->setThighOrientation(convolution);
	m_ui.imgProcessing->setFilterSet(NULL);
	delete thigh;*/
	
	qDebug() << "Minimum energy at theta =" << m_thighFilter->correlate(m_frameInfo);
	
	qDebug() << "Applying filter took" << t.elapsed() << "ms";
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

