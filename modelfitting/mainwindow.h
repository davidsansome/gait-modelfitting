#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <QSettings>

class QTimer;
class Mesh;

#include "ui_mainwindow.h"

class MainWindow : public QDialog
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow();

private slots:
	void openMesh();
	void findCenter();
	void updateViews();
	
private:
	void clearMesh();
	
	Ui_MainWindow m_ui;
	QString m_openDir;
	QTimer* m_redrawTimer;
	QSettings m_settings;
	
	Voxel_Space* m_voxelSpace;
	Mesh* m_mesh;
};

#endif
