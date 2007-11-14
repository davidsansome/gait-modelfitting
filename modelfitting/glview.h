#ifndef GLVIEW_H
#define GLVIEW_H

#include <QGLWidget>
#include <mesh.hh>

#include "types.h"

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
	void setCenter(float2 center) { m_center = center; }

private:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();
	
	void drawTunnel();
	void drawCenter();

	ViewType m_viewType;
	float m_zoom;
	Mesh* m_mesh;
	
	float2 m_center;
	
	static QGLWidget* s_contextWidget;

};

#endif
