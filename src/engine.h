#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGLWidget>
#include <QSize>

#include <Cg/cg.h>
#include <Cg/cgGL.h>

class ShaderPair;
class QTimer;

class Engine : public QGLWidget
{
	Q_OBJECT
public:
	Engine();
	~Engine();

public slots:
	void setImage(const QString& image);
	void loadShaders(const QString& vert, const QString& frag);
	void setShadersEnabled(bool enabled);

private:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();

	void setPixelStep();

	QTimer* m_redrawTimer;

	uint m_texture;
	QSize m_textureSize;

	bool m_shadersEnabled;

	CGcontext m_cgContext;
	CGprofile m_vertProfile;
	CGprofile m_fragProfile;
	ShaderPair* m_shaders;
};

#endif
