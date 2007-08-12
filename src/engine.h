#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGLWidget>
#include <QSize>
#include <QGLPixelBuffer>

#include <Cg/cg.h>
#include <Cg/cgGL.h>

class QTimer;
class FilterSet;

class Engine : public QGLWidget
{
	Q_OBJECT
public:
	Engine();
	~Engine();

public slots:
	void setImage(const QString& image);
	void setFilterSet(FilterSet* filterSet);

private:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	void initMatrices();
	void initCommon();
	void initPbuffer();

	void setShadersEnabled(bool enabled);
	void drawRect();

	QTimer* m_redrawTimer;

	uint m_imageTexture;
	QSize m_imageSize;
	QGLPixelBuffer* m_pbuffer;
	uint m_pbufferTexture;

	bool m_shadersEnabled;

	CGcontext m_cgContext;
	CGprofile m_vertProfile;
	CGprofile m_fragProfile;
	FilterSet* m_filterSet;
};

#endif
