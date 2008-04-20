#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QFutureWatcher>

class QTimer;
class FrameModel;
class Mesh;
class MapReduceProgress;
class EnergyPlotter;
class TimePlotter;
class FftPlotter;
class ErrorCorrection;
class ClassifyDialog;
class FrameSetFilterModel;
class FrameModelFilter;

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
	
	void sliderMoved(int value);
	
	void setInfoParams();
	void getInfoParams();
	void saveParams();
	
	void openDirChanged();
	
	void frameSetActivated(const QModelIndex& index);
	void frameActivated(const QModelIndex& index);
	
	void orthogonalToggled(bool value);
	
private:
	void setupSpinBox(QDoubleSpinBox* spinner, double range, double step);
	void setFrameInfo(FrameInfo* frameInfo);
	
	Ui_MainWindow m_ui;
	QString m_openDir;
	QTimer* m_redrawTimer;
	
	Voxel_Space* m_voxelSpace;
	Mesh* m_mesh;
	FrameInfo* m_frameInfo;
	
	MapReduceProgress* m_progressDialog;
	//EnergyPlotter* m_energyPlotter;
	TimePlotter* m_timePlotter;
	FftPlotter* m_fftPlotter;
	ErrorCorrection* m_errorCorrection;
	ClassifyDialog* m_classifyDialog;
	
	FrameModel* m_model;
	FrameSetFilterModel* m_frameSetFilterModel;
	FrameModelFilter* m_frameFilterModel;
	
	bool m_paramUpdatesDisabled;
};

#endif
