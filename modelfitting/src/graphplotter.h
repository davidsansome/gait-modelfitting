#ifndef GRAPHPLOTTER_H
#define GRAPHPLOTTER_H

#include "frameinfo.h"
#include "ui_graphplotter.h"

#include <QDialog>
#include <QTextStream>
#include <QSettings>

class FrameInfo;

class GraphPlotter : public QDialog
{
	Q_OBJECT
public:
	GraphPlotter(QWidget* parent = 0);
	~GraphPlotter();
	
public slots:
	void setFrameInfo(FrameInfo* info) { m_info = info; }

private slots:
	void okClicked();

private:
	void load();
	void save();
	static QString limbName(int t);
	
	QPair<QString, QString> getFilename();
	void plot(const QString& templateFilename, int limb, const QString& outFilename = QString::null, const QString& termType = QString::null);
	
	QTextStream& writeThighData(Part part, const Params<float>& initialParams, QTextStream& s);
	QTextStream& writeLowerLegData(Part part, const Params<float>& initialParams, QTextStream& s);
	
	FrameInfo* m_info;
	
	QSettings m_settings;
	
	Ui_GraphPlotter m_ui;
};

#endif
