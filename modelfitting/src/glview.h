#ifndef GLVIEW_H
#define GLVIEW_H

#include <QGLWidget>
#include <mesh.hh>

#include "types.h"

class FrameInfo;

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
	
	void setMesh(Mesh* mesh);
	void setViewType(ViewType type);
	void setFrameInfo(const FrameInfo* frameInfo) { m_frameInfo = frameInfo; }
	
	static QGLWidget* s_contextWidget;

private:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	
	void drawTunnel();
	void drawInfo();
	void drawTestCube();

	ViewType m_viewType;
	float m_zoom;
	
	Mesh* m_mesh;
	const FrameInfo* m_frameInfo;
};

#endif
