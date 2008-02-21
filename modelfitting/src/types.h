#ifndef TYPES_H
#define TYPES_H

#include <svl/SVL.h>
#include <QDebug>

QDebug& operator <<(QDebug& s, const Vec3& v);
QDebug& operator <<(QDebug& s, const Vec4& v);
QDebug& operator <<(QDebug& s, const Vec& v);

void glMultMatrix(const Mat4& mat);


#endif
