#include "opengl.h"
#include "glview.h"
#include "frameinfo.h"
#include "model.h"
#include "shader.h"

#include <QDebug>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QGLFramebufferObject>
#include <mesh.hh>

QGLWidget* GLView::s_contextWidget = NULL;
Shader* GLView::s_voxelShader = NULL;
QList<Shader*> GLView::s_ppShaders;

GLView::GLView(QWidget* parent)
	: QGLWidget(parent, s_contextWidget),
	  m_frameInfo(NULL),
	  m_viewType(Front),
	  m_viewDistance(150.0),
	  m_azimuth(0.0),
	  m_zenith(0.0),
	  m_center(0.0, 0.0, 100.0),
	  m_showModel(true),
	  m_showVoxelData(true),
	  m_sceneFbo(NULL)
{
	if (!s_contextWidget)
		s_contextWidget = this;
}

GLView::~GLView()
{
	delete m_sceneFbo;
	qDeleteAll(m_blurTargets);
}

void GLView::setViewType(ViewType type)
{
	m_viewType = type;
}

void GLView::recreateFbos()
{
	delete m_sceneFbo;
	m_sceneFbo = new QGLFramebufferObject(nextPowerOf2(width()), nextPowerOf2(height()), QGLFramebufferObject::Depth);
	glBindTexture(GL_TEXTURE_2D, m_sceneFbo->texture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	qDeleteAll(m_blurTargets);
	m_blurTargets.clear();
	
	QSize size(m_sceneFbo->size() / 2);
	for (int i=0 ; i<3 ; ++i)
	{
		size /= 2;
		
		QGLFramebufferObject* fbo = new QGLFramebufferObject(size, QGLFramebufferObject::Depth);
		glBindTexture(GL_TEXTURE_2D, fbo->texture());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		
		m_blurTargets << fbo;
	}
}

void GLView::initializeGL()
{
	setupWinGLFunctions();
	
	glEnable(GL_NORMALIZE);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_FLAT);
	
	m_quadric = gluNewQuadric();
	
	if (!s_voxelShader)
	{
		s_voxelShader = new Shader(":voxels_vert.glsl", ":voxels_frag.glsl");
		for (int i=0 ; i<5 ; ++i)
			s_ppShaders << new Shader(":pp_vert.glsl", ":pp_pass" + QString::number(i) + ".glsl");
	}
}

void GLView::resizeGL(int width, int height)
{
	recreateFbos();
}

void GLView::paintGL()
{
	// Draw the scene to the scene FBO
	m_sceneFbo->bind();
	glViewport(0, 0, size());
	drawScene();
	m_sceneFbo->release();
	
	m_blurTargets[0]->bind();
	glViewport(0, 0, m_blurTargets[0]->size());
	drawScene();
	m_blurTargets[0]->release();
	
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	
	// Blur the bright bits
	blurPass(s_ppShaders[0], m_blurTargets[0], m_blurTargets[0]);
	blurPass(s_ppShaders[1], m_blurTargets[0], m_blurTargets[0]);
	blurPass(s_ppShaders[2], m_blurTargets[0], m_blurTargets[0]);
	blurPass(s_ppShaders[1], m_blurTargets[0], m_blurTargets[0]);
	blurPass(s_ppShaders[2], m_blurTargets[0], m_blurTargets[0]);
	blurPass(s_ppShaders[1], m_blurTargets[0], m_blurTargets[0]);
	blurPass(s_ppShaders[2], m_blurTargets[0], m_blurTargets[0]);
	blurPass(s_ppShaders[1], m_blurTargets[0], m_blurTargets[0]);
	blurPass(s_ppShaders[2], m_blurTargets[0], m_blurTargets[0]);
	blurPass(s_ppShaders[1], m_blurTargets[0], m_blurTargets[0]);
	
	// Downsample
	blurPass(s_ppShaders[2], m_blurTargets[0], m_blurTargets[1]);
	blurPass(s_ppShaders[1], m_blurTargets[1], m_blurTargets[1]);
	
	blurPass(s_ppShaders[2], m_blurTargets[0], m_blurTargets[2]);
	blurPass(s_ppShaders[1], m_blurTargets[2], m_blurTargets[2]);
	
	// Draw back to the screen
	glViewport(0, 0, width(), height());
	s_ppShaders[4]->bind();
	
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_sceneFbo->texture());
	
	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_blurTargets[0]->texture());
	
	glActiveTexture(GL_TEXTURE2);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_blurTargets[1]->texture());
	
	glActiveTexture(GL_TEXTURE3);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_blurTargets[2]->texture());
	
	glUniform1i(s_ppShaders[4]->uniformLocation("scene"), 0);
	glUniform1i(s_ppShaders[4]->uniformLocation("blur1"), 1);
	glUniform1i(s_ppShaders[4]->uniformLocation("blur2"), 2);
	glUniform1i(s_ppShaders[4]->uniformLocation("blur3"), 3);
	
	drawQuad(float(width()) / m_sceneFbo->width(), float(height()) / m_sceneFbo->height());
	
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE2);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	
	glEnable(GL_DEPTH_TEST);
}

void GLView::blurPass(Shader* shader, QGLFramebufferObject* source, QGLFramebufferObject* target)
{
	glViewport(0, 0, target->size());
	target->bind();
	
	shader->bind();
	glBindTexture(GL_TEXTURE_2D, source->texture());
	glUniform1i(shader->uniformLocation("source"), 0);
	glUniform2f(shader->uniformLocation("pixelStep"), 1.0 / target->width(), 1.0 / target->height());
	drawQuad(1.0, 1.0);
	
	target->release();
}

void GLView::downsamplePass(QGLFramebufferObject* source, QGLFramebufferObject* target)
{
	glViewport(0, 0, target->size());
	target->bind();
	
	s_ppShaders[3]->bind();
	glBindTexture(GL_TEXTURE_2D, source->texture());
	glUniform1i(s_ppShaders[3]->uniformLocation("source"), 0);
	drawQuad(1.0, 1.0);
	
	target->release();
}

void GLView::setupMatrices()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	const float aspectRatio = float(width()) / height();
	const float zNear = 1.0;
	const float zFar = 2000.0;
	
	if (m_viewType == Angle)
		gluPerspective(45.0, aspectRatio, zNear, zFar);
	else
	{
		float x, y;
		switch (m_viewType)
		{
			case Front:    x = XEXTENT; y = ZEXTENT; break;
			case Side:     x = YEXTENT; y = ZEXTENT; break;
			case Overhead: x = YEXTENT; y = XEXTENT; break;
		}
		
		if (aspectRatio < x/y)
		{
			float scale = x/width();
			glOrtho(-x/2.0, x/2.0, -scale*height()/2.0, scale*height()/2.0, zNear, zFar);
		}
		else
		{
			float scale = y/height();
			glOrtho(-scale*width()/2.0, scale*width()/2.0, -y/2.0, y/2.0, zNear, zFar);
		}
	}
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GLView::setupCamera()
{
	switch (m_viewType)
	{
		case Front:    gluLookAt(0.0, 200.0, 100.0, 0.0, 0.0, 100.0, 0.0,  0.0, 1.0);             break;
		case Side:     gluLookAt(200.0, 0.0, 100.0, 0.0, 0.0, 100.0, 0.0,  0.0, 1.0);             break;
		case Overhead: gluLookAt(0.0, 0.0, 200.0,   0.0, 0.0, 100.0, -1.0, 0.0, 0.0);             break;
		case Angle:
			gluLookAt(m_center[0] + cos(m_azimuth) * cos(m_zenith) * m_viewDistance,
			          m_center[1] + sin(m_azimuth) * cos(m_zenith) * m_viewDistance,
			          m_center[2] + sin(m_zenith) * m_viewDistance,
			          m_center[0], m_center[1], m_center[2],
			          0.0, 0.0, 1.0);
			break;
	}
	
	float lightPos[4] = {0.0, MAXY, ZEXTENT, 0.0};
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}

void GLView::drawScene()
{
	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Shader::unbind();
	
	setupMatrices();
	setupCamera();
	
	if (!m_frameInfo)
		return;
	
	if (m_showVoxelData)
	{
		s_voxelShader->bind();
		m_frameInfo->mesh()->draw();
		s_voxelShader->unbind();
	}
	
	drawTunnel();
	
	if (m_showModel)
		drawInfo();
}

void GLView::drawTunnel()
{
	glColor4f(0.0, 1.0, 0.0, 1.0);
	
	// Left side
	glBegin(GL_LINE_LOOP);
		glVertex3f(MINX, MINY, 0.0);
		glVertex3f(MINX, MAXY, 0.0);
		glVertex3f(MINX, MAXY, ZEXTENT);
		glVertex3f(MINX, MINY, ZEXTENT);
	glEnd();
	
	// Right side
	glBegin(GL_LINE_LOOP);
		glVertex3f(MAXX, MINY, 0.0);
		glVertex3f(MAXX, MAXY, 0.0);
		glVertex3f(MAXX, MAXY, ZEXTENT);
		glVertex3f(MAXX, MINY, ZEXTENT);
	glEnd();
	
	// Lines going across
	glBegin(GL_LINES);
		glVertex3f(MINX, MAXY, 0.0);
		glVertex3f(MAXX, MAXY, 0.0);
		glVertex3f(MINX, MINY, 0.0);
		glVertex3f(MAXX, MINY, 0.0);
		glVertex3f(MINX, MAXY, ZEXTENT);
		glVertex3f(MAXX, MAXY, ZEXTENT);
		glVertex3f(MINX, MINY, ZEXTENT);
		glVertex3f(MAXX, MINY, ZEXTENT);
	glEnd();
}

void GLView::drawTestCube()
{
	glBegin(GL_QUADS);
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-0.5f, 0.5f, -0.5f);
		glVertex3f(0.5f, 0.5f, -0.5f);
		glVertex3f(0.5f, 0.5f, 0.5f);
		glVertex3f(-0.5f, 0.5f, 0.5f);

		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.5f, 0.5f, 0.5f);
		glVertex3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, 0.5f);
		glVertex3f(-0.5f, -0.5f, 0.5f);

		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.5f, -0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, 0.5f);
		glVertex3f(0.5f, -0.5f, -0.5f);
		glVertex3f(-0.5f, -0.5f, -0.5f);

		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(-0.5f, -0.5f, -0.5f);
		glVertex3f(0.5f, -0.5f, -0.5f);
		glVertex3f(0.5f, 0.5f, -0.5f);
		glVertex3f(-0.5f, 0.5f, -0.5f);

		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(-0.5f, 0.5f, -0.5f);
		glVertex3f(-0.5f, 0.5f, 0.5f);
		glVertex3f(-0.5f, -0.5f, 0.5f);
		glVertex3f(-0.5f, -0.5f, -0.5f);

		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.5f, 0.5f, -0.5f);
		glVertex3f(0.5f, -0.5f, -0.5f);
		glVertex3f(0.5f, -0.5f, 0.5f);
	glEnd();
}

void GLView::drawQuad(float width, float height)
{
	glBegin(GL_QUADS);
		glMultiTexCoord2f(GL_TEXTURE0, 0.0, height);
		glMultiTexCoord2f(GL_TEXTURE1, 0.0, 1.0);
		glVertex2f(-1.0, 1.0);
		
		glMultiTexCoord2f(GL_TEXTURE0, width, height);
		glMultiTexCoord2f(GL_TEXTURE1, 1.0, 1.0);
		glVertex2f(1.0, 1.0);
		
		glMultiTexCoord2f(GL_TEXTURE0, width, 0.0);
		glMultiTexCoord2f(GL_TEXTURE1, 1.0, 0.0);
		glVertex2f(1.0, -1.0);
		
		glMultiTexCoord2f(GL_TEXTURE0, 0.0, 0.0);
		glMultiTexCoord2f(GL_TEXTURE1, 0.0, 0.0);
		glVertex2f(-1.0, -1.0);
	glEnd();
}

void GLView::drawInfo()
{
	if (m_frameInfo == NULL || !m_frameInfo->hasModelInformation())
		return;
	
	const Vec2 center(m_frameInfo->center());
	int highestPoint = m_frameInfo->highestPoint();
	
	glColor4f(1.0, 1.0, 1.0, 1.0);
	glDisable(GL_CULL_FACE);
	
	glPushMatrix();
		glTranslatef(MINX, MINY, 0.0);
		
		if (m_viewType == Overhead)
		{
			glBegin(GL_POINTS);
				glVertex3f(center[0], center[1], 0.0);
			glEnd();
		}
		else
		{
			glBegin(GL_LINES);
				glVertex3f(center[0], center[1], 0.0);
				glVertex3f(center[0], center[1], highestPoint);
			glEnd();
		}
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPushMatrix();
			glMultMatrix(m_frameInfo->limbMatrix(LeftLeg, Thigh));
			FrameInfo::thighModel()->draw();
		glPopMatrix();
		
		glPushMatrix();
			glMultMatrix(m_frameInfo->limbMatrix(LeftLeg, LowerLeg));
			FrameInfo::thighModel()->draw();
		glPopMatrix();
		
		glPushMatrix();
			glMultMatrix(m_frameInfo->limbMatrix(RightLeg, Thigh));
			FrameInfo::thighModel()->draw();
		glPopMatrix();
		
		glPushMatrix();
			glMultMatrix(m_frameInfo->limbMatrix(RightLeg, LowerLeg));
			FrameInfo::thighModel()->draw();
		glPopMatrix();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glPopMatrix();
}

int GLView::extent() const
{
	switch (m_viewType)
	{
		case Front:    return int(YEXTENT);
		case Side:     return int(XEXTENT);
		case Overhead: return int(ZEXTENT);
		default:       return -1;
	}
}

void GLView::setCrossSection(int value)
{
	makeCurrent();
	
	if (value == -1)
	{
		glDisable(GL_CLIP_PLANE0);
		glDisable(GL_CLIP_PLANE1);
		return;
	}
	glEnable(GL_CLIP_PLANE0);
	glEnable(GL_CLIP_PLANE1);
	
	double equation[4] = {0.0, 0.0, 0.0, 0.0};
	switch (m_viewType)
	{
		case Front:    equation[1] = 1.0; value += int(MINY); break;
		case Side:     equation[0] = 1.0; value += int(MINX); break;
		case Overhead: equation[2] = 1.0;                     break;
	}
	
	equation[3] = - (value - 0.1);
	glClipPlane(GL_CLIP_PLANE0, equation);
	
	equation[0] *= -1.0;
	equation[1] *= -1.0;
	equation[2] *= -1.0;
	equation[3] = value + 1.1;
	glClipPlane(GL_CLIP_PLANE1, equation);
}

void GLView::mousePressEvent(QMouseEvent* e)
{
	m_mouseDownPos = e->pos();
	m_mouseDownAzimuth = m_azimuth;
	m_mouseDownZenith = m_zenith;
	m_mouseDownCenter = m_center;
}

void GLView::mouseMoveEvent(QMouseEvent* e)
{
	if (e->modifiers() & Qt::ShiftModifier)
	{
		Vec3 viewVec(cos(m_azimuth) * cos(m_zenith),
		             sin(m_azimuth) * cos(m_zenith),
		             sin(m_zenith));
		viewVec = norm(viewVec);
		Vec3 upVec(0.0, 0.0, 1.0);
		
		Vec3 screenRight = cross(viewVec, upVec);
		Vec3 screenUp = cross(screenRight, viewVec);
		
		m_center = m_mouseDownCenter +
		           screenUp * 0.5 * (e->pos().y() - m_mouseDownPos.y()) +
		           screenRight * 0.5 * (e->pos().x() - m_mouseDownPos.x());
	}
	else
	{
		m_azimuth = m_mouseDownAzimuth + (m_mouseDownPos.x() - e->pos().x()) * 0.01;
		m_zenith = m_mouseDownZenith + (e->pos().y() - m_mouseDownPos.y()) * 0.01;
		
		m_zenith = qBound(float(-M_PI_2 + 0.001), m_zenith, float(M_PI_2 - 0.001));
	}
}

void GLView::wheelEvent(QWheelEvent* e)
{
	m_viewDistance = qMax(2.0, m_viewDistance - float(e->delta()) / 5.0);
}

