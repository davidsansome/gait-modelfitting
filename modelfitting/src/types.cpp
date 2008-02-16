#include "types.h"

#include <GL/gl.h>

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

void glMultMatrix(const Mat4& mat)
{
#ifdef VL_FLOAT
	glMultMatrixf(trans(mat).Ref());
#else
	glMultMatrixd(trans(mat).Ref());
#endif
}
