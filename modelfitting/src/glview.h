#ifndef GLVIEW_H
#define GLVIEW_H

#include <QGLWidget>
#include <GL/glu.h>

#include "types.h"

class FrameInfo;

class QMouseEvent;
class QWheelEvent;

class GLView : public QGLWidget
{
	Q_OBJECT
public:
	enum ViewType
	{
		Front,
		Side,
		Overhead,
		Angle
	};
	
	GLView(QWidget* parent = NULL);
	~GLView();
	
	void setViewType(ViewType type);
	void setCrossSection(int value);
	void setFrameInfo(const FrameInfo* frameInfo) { m_frameInfo = frameInfo; }
	
	int extent() const;
	
	void mouseMoveEvent(QMouseEvent* e);
	void mousePressEvent(QMouseEvent* e);
	void wheelEvent(QWheelEvent* e);
	
	static QGLWidget* s_contextWidget;

private:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	
	void drawTunnel();
	void drawInfo();
	void drawTestCube();

	ViewType m_viewType;
	float m_viewDistance;
	
	const FrameInfo* m_frameInfo;
	
	GLUquadric* m_quadric;
	
	QPoint m_mouseDownPos;
	float m_azimuth;
	float m_mouseDownAzimuth;
	float m_zenith;
	float m_mouseDownZenith;
	Vec3 m_center;
	Vec3 m_mouseDownCenter;
};

#endif
