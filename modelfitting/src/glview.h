#ifndef GLVIEW_H
#define GLVIEW_H

#include <QGLWidget>

#include "types.h"

class FrameInfo;
class Shader;

class QMouseEvent;
class QWheelEvent;
class QGLFramebufferObject;

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
	
	int extent() const;
	
	void mouseMoveEvent(QMouseEvent* e);
	void mousePressEvent(QMouseEvent* e);
	void wheelEvent(QWheelEvent* e);
	
	static QGLWidget* s_contextWidget;

public slots:
	void setViewType(ViewType type);
	void setCrossSection(int value);
	void setFrameInfo(const FrameInfo* frameInfo) { m_frameInfo = frameInfo; }
	
	void setVoxelDataVisible(bool showVoxelData) { m_showVoxelData = showVoxelData; }
	void setModelVisible(bool showModel) { m_showModel = showModel; }

private:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	
	void recreateFbos();
	void setupMatrices();
	void setupCamera();
	
	void drawScene();
	void drawTunnel();
	void drawInfo();
	void drawTestCube();
	
	void drawQuad(float width, float height);
	void blurPass(Shader* shader, QGLFramebufferObject* source, QGLFramebufferObject* target);
	void downsamplePass(QGLFramebufferObject* source, QGLFramebufferObject* target);
	
	ViewType m_viewType;
	float m_viewDistance;
	
	bool m_showModel;
	bool m_showVoxelData;
	
	const FrameInfo* m_frameInfo;
	
	GLUquadric* m_quadric;
	
	QPoint m_mouseDownPos;
	float m_azimuth;
	float m_mouseDownAzimuth;
	float m_zenith;
	float m_mouseDownZenith;
	Vec3 m_center;
	Vec3 m_mouseDownCenter;
	
	static Shader* s_voxelShader;
	static QList<Shader*> s_ppShaders;
	
	QGLFramebufferObject* m_sceneFbo;
	QList<QGLFramebufferObject*> m_blurTargets;
};

#endif
