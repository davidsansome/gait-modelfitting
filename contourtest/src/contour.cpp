#include "contour.h"

#include <QPaintEvent>
#include <QKeyEvent>
#include <QPainter>
#include <cmath>
#include <svl/SVL.h>

Contour::Contour(const QString& image, QWidget* parent)
	: QWidget(parent),
	  m_image(image)
{
	resetSpline();
	
	resize(m_image.size());
}

Contour::~Contour()
{
}

void Contour::resetSpline()
{
	const int pointCount = 10;
	
	m_center = QPoint(m_image.size().width() / 2, m_image.size().height() / 2);
	float radius = 15.0;
	
	for (int i=0 ; i<pointCount ; ++i)
	{
		float theta = (M_PI * 2.0) / pointCount * i;
		int x = m_center.x() + int(radius * cos(theta));
		int y = m_center.y() + int(radius * sin(theta));
		
		m_spline.append(QPoint(x, y));
	}
}

void Contour::advanceSpline()
{
	QList<QPoint> newSpline;
	
	for (int i=0 ; i<m_spline.count() ; ++i)
	{
		QPoint oldPoint(m_spline[i]);
		QPoint bestPoint;
		float bestEnergy = 0.0;
		for (int x=-3 ; x<=3 ; ++x)
		{
			for (int y=-3 ; y<=3 ; ++y)
			{
				QPoint newPoint = oldPoint + QPoint(x, y);
				float newEnergy =
					continuityEnergy(i, oldPoint, newPoint) +
					balloonEnergy(i, oldPoint, newPoint) +
					externalEnergy(i, oldPoint, newPoint);
				
				if (newEnergy > bestEnergy)
				{
					bestEnergy = newEnergy;
					bestPoint = newPoint;
				}
			}
		}
		
		newSpline.append(bestPoint);
	}
	
	m_spline = newSpline;
	update();
}

void Contour::paintEvent(QPaintEvent* e)
{
	QPainter p(this);
	p.drawImage(QPoint(0,0), m_image);
	
	p.setPen(Qt::white);
	for (int i=0 ; i<m_spline.count() ; ++i)
		p.drawLine(m_spline[i], m_spline[(i+1) % m_spline.count()]);
	
	QPen pointPen(Qt::green);
	pointPen.setWidth(4);
	p.setPen(pointPen);
	for (int i=0 ; i<m_spline.count() ; ++i)
		p.drawPoint(m_spline[i]);
}

void Contour::keyPressEvent(QKeyEvent* e)
{
	if (e->key() == Qt::Key_Space)
		advanceSpline();
}


float Contour::continuityEnergy(int i, const QPoint& oldPoint, const QPoint& newPoint)
{
	return 0.0;
}

float Contour::balloonEnergy(int i, const QPoint& oldPoint, const QPoint& newPoint)
{
	Vec2 normal = norm(Vec2(oldPoint.x(), oldPoint.y()) - Vec2(m_center.x(), m_center.y()));
	Vec2 thisVec = Vec2(newPoint.x(), newPoint.y()) - Vec2(oldPoint.x(), oldPoint.y());
	
	return dot(normal, thisVec);
}

float Contour::externalEnergy(int i, const QPoint& oldPoint, const QPoint& newPoint)
{
	return 0.0;
}

