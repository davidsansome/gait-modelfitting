#ifndef CONTOUR_H
#define CONTOUR_H

#include <QWidget>
#include <QList>
#include <QImage>

#include <svl/SVL.h>

class QPaintEvent;
class QKeyEvent;

class Contour : public QWidget
{
	Q_OBJECT
public:
	Contour(const QString& image, QWidget* parent);
	~Contour();

public slots:
	void resetSpline();
	void advanceSpline();

protected:
	void paintEvent(QPaintEvent* e);
	void keyPressEvent(QKeyEvent* e);
	
private:
	QPoint sp(int i) const;
	Vec2 spv(int i) const;
	float continuityEnergy(int i, const QPoint& oldPoint, const QPoint& newPoint);
	float balloonEnergy(int i, const QPoint& oldPoint, const QPoint& newPoint);
	float shapeEnergy(int i, const QPoint& oldPoint, const QPoint& newPoint);
	float externalEnergy(int i, const QPoint& oldPoint, const QPoint& newPoint);

	QList<QPoint> m_spline;
	QImage m_image;
	
	QPoint m_center;
};

#endif
