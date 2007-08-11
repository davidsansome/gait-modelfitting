#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGLWidget>

class Engine : public QGLWidget
{
	Q_OBJECT
public:
	Engine();
	~Engine();

	void setImage(const QString& image);

private:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	uint texture;
};

#endif
