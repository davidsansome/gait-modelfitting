#include "types.h"

QDebug& operator <<(QDebug& s, const Vec3& v)
{
	s << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
	return s;
}

QDebug& operator <<(QDebug& s, const Vec4& v)
{
	s << "(" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ")";
	return s;
}

