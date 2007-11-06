#ifndef GLVIEW_H
#define GLVIEW_H

#include <QGLWidget>

class GLView : public QGLWidget
{
	Q_OBJECT
public:
	GLView(QWidget* parent = NULL);
	~GLView();

private:
	void resizeGL(int width, int height);
	void initializeGL();
	void paintGL();
};

#endif
