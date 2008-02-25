#ifndef GRAPHPLOTTER_H
#define GRAPHPLOTTER_H

#include "frameinfo.h"

#include <QObject>
#include <QTextStream>
#include <QSettings>

class FrameInfo;

class GraphPlotter : public QObject
{
	Q_OBJECT
public:
	GraphPlotter(QObject* parent = 0);
	~GraphPlotter();
	
public slots:
	void setFrameInfo(FrameInfo* info) { m_info = info; }
	void setLimb(int limb) { m_limb = limb; }
	
	void display();
	void save();

private:
	static QString limbName(int t);
	
	QPair<QString, QString> getFilename();
	void plot(const QString& templateFilename, const QString& outFilename = QString::null, const QString& termType = QString::null);
	
	QTextStream& writeThighData(Part part, const Params<float>& initialParams, QTextStream& s);
	QTextStream& writeLowerLegData(Part part, const Params<float>& initialParams, QTextStream& s);
	
	FrameInfo* m_info;
	int m_limb;
	
	QSettings m_settings;
	QString m_saveDirectory;
};

#endif
