#include "convolution.h"
#include "shaderpair.h"
#include "dataunit.h"
#include "filter.h"
#include "frameinfo.h"
#include <vspace.hh>
#include <math.h>

#include <QGLFramebufferObject>
#include <QDebug>

#define THIGH_XEXTENT 30.0
#define THIGH_YEXTENT 30.0
#define THIGH_ZEXTENT 50.0

Convolution::Convolution(Filter* filter, FrameInfo* info)
	: FilterSet(),
	  m_filter(filter),
	  m_info(info)
{
}

Convolution::~Convolution()
{
	delete m_shaders;
	delete m_fboDU;
}

void Convolution::thighTransform(const FrameInfo* info, bool scale, bool offsetCenter)
{
	// Scale down to voxel space
	if (scale)
		glScalef
		(
			1.0/info->vspace()->x_size,
			1.0/info->vspace()->y_size,
			1.0/info->vspace()->z_size
		);
	
	// Translate to the location of the thigh
	glTranslatef
	(
		info->center()[0] + info->xWidth() / 6.0,
		info->center()[1],
		info->highestPoint() / 2.0
	);
	
	// Scale to thigh space
	glScalef
	(
		THIGH_XEXTENT,
		THIGH_YEXTENT,
		-THIGH_ZEXTENT
	);
	
	if (offsetCenter)
		glTranslatef(-0.5, -0.5, 0.0);
}

void Convolution::init(CGcontext context, CGprofile vertProfile, CGprofile fragProfile, const QSize& imageSize)
{
	m_shaders = new ShaderPair(context);
	m_shaders->loadVertexProgram("shaders/vertexBasic.cg", vertProfile);
	m_shaders->loadFragmentProgram("shaders/convolution.cg", fragProfile);

	m_imageSize = imageSize;
	cgGLSetParameter1f(cgGetNamedParameter(m_shaders->frag(), "pixelStep"), 1.0f / float(imageSize.width()));
	
	// Make a matrix
	glPushMatrix();
		glLoadIdentity();
		thighTransform(m_info);
		
		// (Shader does a rotation about this point)
		float rotPoint[] = { THIGH_XEXTENT/2.0, THIGH_YEXTENT/2.0, 0.0 };
		
		// The rest of the transformations are done by the shader
		CGparameter param = cgGetNamedParameter(m_shaders->frag(), "matrix");
		cgGLSetStateMatrixParameter(param, CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
		
		param = cgGetNamedParameter(m_shaders->frag(), "rotPoint");
		cgGLSetParameter3fv(param, rotPoint);
	glPopMatrix();
	
	m_fbo = new QGLFramebufferObject(imageSize);
	m_fboDU = new DataUnit(m_fbo);
}

void Convolution::setDataUnit(int index, DataUnit* du)
{
	switch (index)
	{
		case 1: m_output = du; break;
	}
}

void Convolution::bind(int pass)
{
	switch (pass)
	{
		case 0:
			m_filter->bind(0);
			m_info->bindTexture(1);
			m_output->bindOutput();
			m_shaders->bind();
			break;
		case 1:
			m_filter->bind(0);
			m_info->bindTexture(1);
			m_fboDU->bindOutput();
			m_shaders->bind();
			break;
	}
}

void Convolution::release(int pass)
{
	switch (pass)
	{
		case 0:
			m_output->releaseOutput();
			break;
		case 1:
			m_fboDU->releaseOutput();
			break;
	}
}

QPointF Convolution::mean() const
{
	QImage result = m_fbo->toImage();
	result.save("/home/david/test.png", "PNG");
	
	float totalX = 0.0;
	float totalY = 0.0;
	float totalIntensity = 0.0;
	
	int w = result.width();
	int h = result.height();
	for (int x=0 ; x<w ; ++x)
	{
		for (int y=0 ; y<h ; ++y)
		{
			float p = float(qRed(result.pixel(x, y))) / 255.0;
			totalX += float(x) * p;
			totalY += float(y) * p;
			totalIntensity += p;
		}
	}
	
	qDebug() << totalX/totalIntensity;
	
	return QPointF
	(
		totalX/(w*totalIntensity) * (M_PI/2.0) - (M_PI/4.0),
		totalY/(h*totalIntensity) * (M_PI/4.0) - (M_PI/8.0)
	);
}


