#ifndef IMGPROCESSING_H
#define IMGPROCESSING_H

#include <QGLWidget>
#include <QSize>
#include <QGLFramebufferObject>

#include <Cg/cg.h>
#include <Cg/cgGL.h>

class QTimer;
class FilterSet;
class DataUnit;

class ImgProcessing : public QGLWidget
{
	Q_OBJECT
public:
	ImgProcessing(QWidget* parent, const QGLWidget* shareWidget);
	~ImgProcessing();

public slots:
	void setFilterSet(FilterSet* filterSet);

signals:
	void setupReady();

private:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	void setShadersEnabled(bool enabled);
	void drawRect();

	QTimer* m_redrawTimer;

	DataUnit* m_framebuffer;
	QSize m_imageSize;

	bool m_shadersEnabled;

	CGcontext m_cgContext;
	CGprofile m_vertProfile;
	CGprofile m_fragProfile;
	FilterSet* m_filterSet;
};

#endif
