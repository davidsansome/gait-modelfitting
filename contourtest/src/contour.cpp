#include "contour.h"

#include <QPaintEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QDebug>
#include <cmath>
#include <limits>
#include <svl/SVL.h>

Vec2 v2(const QPoint& point)
{
	return Vec2(point.x(), point.y());
}

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
		float bestEnergy = std::numeric_limits<float>::max();
		for (int x=-3 ; x<=3 ; ++x)
		{
			for (int y=-3 ; y<=3 ; ++y)
			{
				QPoint newPoint = oldPoint + QPoint(x, y);
				float newEnergy =
					continuityEnergy(i, oldPoint, newPoint) +
					balloonEnergy(i, oldPoint, newPoint) +
					10.0 * externalEnergy(i, oldPoint, newPoint);
				
				if (newEnergy < bestEnergy)
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
		p.drawLine(sp(i), sp(i+1));
	
	QPen pointPen(Qt::green);
	pointPen.setWidth(4);
	p.setPen(pointPen);
	for (int i=0 ; i<m_spline.count() ; ++i)
		p.drawPoint(sp(i));
}

void Contour::keyPressEvent(QKeyEvent* e)
{
	if (e->key() == Qt::Key_Space)
		advanceSpline();
}

QPoint Contour::sp(int i) const
{
	while (i < 0) i += m_spline.count();
	while (i >= m_spline.count()) i -= m_spline.count();
	
	return m_spline[i];
}

Vec2 Contour::spv(int i) const
{
	return v2(sp(i));
}

float Contour::continuityEnergy(int i, const QPoint& oldPoint, const QPoint& newPoint)
{
	Vec2 lastPoint = spv(i-1);
	Vec2 diff = lastPoint - v2(newPoint);
	
	// TODO: Calculate this once per run
	float averageDistance = 0.0;
	for (int j=0 ; j<m_spline.count() ; ++j)
		averageDistance += len(spv(j+1) - spv(j));
	averageDistance /= m_spline.count();
	
	return abs(averageDistance - len(diff));
}

float Contour::balloonEnergy(int i, const QPoint& oldPoint, const QPoint& newPoint)
{
	Vec2 normal = norm(v2(oldPoint) - v2(m_center));
	Vec2 thisVec = v2(oldPoint) - v2(newPoint);
	
	return dot(normal, thisVec);
}

float Contour::externalEnergy(int i, const QPoint& oldPoint, const QPoint& newPoint)
{
	QRgb color = m_image.pixel(newPoint);
	
	return (qRed(color) == 0) ? 0.0 : 1.0;
}

