#ifndef TYPES_H
#define TYPES_H

#include <svl/SVL.h>
#include <QDebug>
#include <complex>

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


#endif
