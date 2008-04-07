#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QFutureWatcher>

class QTimer;
class Mesh;
class MapReduceProgress;
class EnergyPlotter;
class TimePlotter;
class FftPlotter;
class ErrorCorrection;
class ClassifyDialog;

#include "ui_mainwindow.h"
#include "types.h"
#include "frameinfo.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow();
	~MainWindow();

private slots:
	void initializeGL();
	void openDirectory();
	bool recalculate();
	void recalculateAll();
	void classify();
	void updateViews();
	void loadSelectedFile();
	
	void sliderMoved(int value);
	
	void updateFileListing();
	
	void setInfoParams();
	void getInfoParams();
	
private:
	void setupSpinBox(QDoubleSpinBox* spinner, double range, double step);
	
	Ui_MainWindow m_ui;
	QString m_openDir;
	QTimer* m_redrawTimer;
	
	Voxel_Space* m_voxelSpace;
	Mesh* m_mesh;
	FrameSet* m_frameSet;
	FrameInfo* m_frameInfo;
	
	MapReduceProgress* m_progressDialog;
	EnergyPlotter* m_energyPlotter;
	TimePlotter* m_timePlotter;
	FftPlotter* m_fftPlotter;
	ErrorCorrection* m_errorCorrection;
	ClassifyDialog* m_classifyDialog;
	
	bool m_paramUpdatesDisabled;
};

#endif
