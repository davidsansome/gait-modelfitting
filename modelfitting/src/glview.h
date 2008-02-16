#ifndef GLVIEW_H
#define GLVIEW_H

#include <QGLWidget>
#include <mesh.hh>
#include <GL/glu.h>

#include "types.h"

class FrameInfo;
class MeshFilter;

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
	void setCrossSection(int value);
	void setFrameInfo(const FrameInfo* frameInfo) { m_frameInfo = frameInfo; }
	void setThighFilter(const MeshFilter* filter) { m_thighFilter = filter; }
	
	int extent() const;
	
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
	const MeshFilter* m_thighFilter;
	
	GLUquadric* m_quadric;
};

#endif
