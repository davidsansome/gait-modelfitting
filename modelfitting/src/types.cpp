#include "types.h"
#include "opengl.h"

#include <QRect>
#include <QSize>
#include <QPoint>

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

QDebug& operator <<(QDebug& s, const Vec& v)
{
	s << "(";
	for (int i=0 ; i<v.Elts() ; i++)
		s << v[i] << ",";
	s << ")";
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

void glViewport(const QRect& rect)
{
	glViewport(rect.x(), rect.y(), rect.width(), rect.height());
}

void glViewport(const QPoint& pos, const QSize& size)
{
	glViewport(pos.x(), pos.y(), size.width(), size.height());
}

void glViewport(int x, int y, const QSize& size)
{
	glViewport(x, y, size.width(), size.height());
}

int nextPowerOf2(int n)
{
	int ret = 1;
	while (ret < n)
		ret *= 2;
	return ret;
}
