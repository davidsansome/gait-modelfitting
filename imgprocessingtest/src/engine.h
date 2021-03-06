#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGLWidget>
#include <QSize>
#include <QGLFramebufferObject>

#include <Cg/cg.h>
#include <Cg/cgGL.h>

class QTimer;
class FilterSet;
class DataUnit;

class Engine : public QGLWidget
{
	Q_OBJECT
public:
	Engine();
	~Engine();

public slots:
	void setImage(const QString& image);
	void setFilterSet(FilterSet* filterSet);

private slots:
	void updateFps();

private:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	void setShadersEnabled(bool enabled);
	void drawRect();

	QTimer* m_redrawTimer;

	DataUnit* m_image;
	DataUnit* m_framebuffer;
	QSize m_imageSize;

	bool m_shadersEnabled;

	CGcontext m_cgContext;
	CGprofile m_vertProfile;
	CGprofile m_fragProfile;
	FilterSet* m_filterSet;

	int m_frameCounter;
};

#endif
