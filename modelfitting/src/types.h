#ifndef TYPES_H
#define TYPES_H

#include <svl/SVL.h>
#include <QDebug>
#include <complex>

class QPoint;
class QRect;
class QSize;

#define ROUND(x) int((x) + ((x) > 0.0 ? 0.5 : -0.5))

QDebug& operator <<(QDebug& s, const Vec3& v);
QDebug& operator <<(QDebug& s, const Vec4& v);
QDebug& operator <<(QDebug& s, const Vec& v);

template<typename T>
QDebug& operator <<(QDebug& s, const std::complex<T>& v)
{
	s.nospace() << "Complex(" << v.real() << ", " << v.imag() << ") ";
	return s;
}

void glMultMatrix(const Mat4& mat);
void glViewport(const QRect& rect);
void glViewport(const QPoint& pos, const QSize& size);
void glViewport(int x, int y, const QSize& size);

int nextPowerOf2(int n);


#endif
