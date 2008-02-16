#ifndef TYPES_H
#define TYPES_H

#include <tr1/array>
#include <stdarg.h>
#include <svl/SVL.h>
#include <QDebug>

typedef std::tr1::array<float, 2> float2;
typedef std::tr1::array<float, 3> float3;

template<int N>
std::tr1::array<float, N> floatn(double f, ...)
{
	va_list ap;
	va_start(ap, f);
	std::tr1::array<float, N> ret;
	ret[0] = f;
	for (int i=1; i<N ; ++i)
		ret[i] = va_arg(ap, double);
	va_end(ap);
	return ret;
}

QDebug& operator <<(QDebug& s, const Vec3& v);
QDebug& operator <<(QDebug& s, const Vec4& v);


#endif
