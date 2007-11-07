#include "mainwindow.h"

#include "mesh.hh"

#include <QFileDialog>
#include <QTimer>
#include <QDebug>

MainWindow::MainWindow()
	: QDialog(NULL),
	  m_mesh(NULL)
{
	m_ui.setupUi(this);
	
	connect(m_ui.openButton, SIGNAL(clicked()), SLOT(openMesh()));
	connect(m_ui.findCenterButton, SIGNAL(clicked()), SLOT(findCenter()));
	
	m_ui.front->setViewType(GLView::Front);
	m_ui.side->setViewType(GLView::Side);
	m_ui.overhead->setViewType(GLView::Overhead);
	m_ui.angle->setViewType(GLView::Angle);
	
	m_redrawTimer = new QTimer(this);
	m_redrawTimer->setSingleShot(true);
	connect(m_redrawTimer, SIGNAL(timeout()), SLOT(updateViews()));
	m_redrawTimer->start(0);
	
	m_openDir = m_settings.value("OpenDir", QDir::homePath()).toString();
}

MainWindow::~MainWindow()
{
	clearMesh();
}

void MainWindow::clearMesh()
{
	if (!m_mesh)
		return;
	m_mesh->draw_destroy();
	delete m_mesh;
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
	
	Voxel_Space spc(fileName.toAscii().data());
	clearMesh();
	m_mesh = new Mesh(spc);
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
}

