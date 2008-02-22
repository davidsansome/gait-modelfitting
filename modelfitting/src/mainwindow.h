#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <QSettings>
#include <QFutureWatcher>

class QTimer;
class Mesh;
class MapReduceProgress;

#include "ui_mainwindow.h"
#include "types.h"
#include "frameinfo.h"

class MainWindow : public QDialog
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow();

private slots:
	void initializeGL();
	void openDirectory();
	void recalculate();
	void recalculateAll();
	void updateViews();
	void loadSelectedFile();
	
	void sliderMoved(int value);
	
private:
	void updateFileListing();
	
	Ui_MainWindow m_ui;
	QString m_openDir;
	QTimer* m_redrawTimer;
	QSettings m_settings;
	
	Voxel_Space* m_voxelSpace;
	Mesh* m_mesh;
	FrameInfo* m_frameInfo;
	
	MapReduceProgress* m_progressDialog;
};

#endif
