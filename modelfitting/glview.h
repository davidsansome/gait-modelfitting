#ifndef GLVIEW_H
#define GLVIEW_H

#include <QGLWidget>
#include <mesh.hh>

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

private:
	void initializeGL();
	void resizeGL(int width, int height);
	void paintGL();

	ViewType m_viewType;
	float m_zoom;
	Mesh* m_mesh;
	
	static QGLWidget* s_contextWidget;

};

#endif
