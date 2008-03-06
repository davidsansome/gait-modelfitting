#ifndef GRAPHPLOTTER_H
#define GRAPHPLOTTER_H

#include "frameinfo.h"
#include "ui_graphplotter.h"

#include <QDialog>
#include <QTextStream>
#include <QSettings>

class FrameInfo;
class QTemporaryFile;

class GraphPlotter : public QDialog
{
	Q_OBJECT
public:
	GraphPlotter(QWidget* parent = 0);
	virtual ~GraphPlotter();
	
	FrameInfo* frameInfo() const { return m_info; }
	
public slots:
	void setFrameInfo(FrameInfo* info) { m_info = info; }
	void exec();
	
protected:
	virtual QString templateName(bool displayOnly) const = 0;
	virtual void plotData(const QString& outFilename) = 0;
	virtual void replaceTokens(QByteArray& commands) = 0;
	
	QTextStream& openTempFile();
	void saveGraph(const QString& filename);
	
	Ui_GraphPlotter m_ui;

private slots:
	void okClicked();

private:
	void load();
	void save();
	
	QPair<QString, QString> getFilename();
	void plot(const QString& templateFilename, int limb, const QString& outFilename = QString::null, const QString& termType = QString::null);
	
	FrameInfo* m_info;
	QSettings m_settings;
	
	QTextStream m_stream;
	QTemporaryFile* m_tempFile;
	QString m_tempFileName;
	QString m_termType;
	QString m_extension;
};

#endif
