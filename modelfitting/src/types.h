#ifndef TYPES_H
#define TYPES_H

#include <svl/SVL.h>
#include <QDebug>

#define ROUND(x) int((x) + ((x) > 0.0 ? 0.5 : -0.5))

QDebug& operator <<(QDebug& s, const Vec3& v);
QDebug& operator <<(QDebug& s, const Vec4& v);
QDebug& operator <<(QDebug& s, const Vec& v);

void glMultMatrix(const Mat4& mat);


#endif
