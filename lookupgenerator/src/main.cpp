#include <QCoreApplication>
#include <QMultiMap>
#include <QDebug>
#include <QPoint>
#include <QFile>
#include <QDataStream>

#include <cmath>

template<typename T>
class Vec3
{
public:
	Vec3(T cx, T cy, T cz) : x(cx), y(cy), z(cz) {}
	Vec3() {}
	
	T x;
	T y;
	T z;
};

template<typename T>
QDebug& operator <<(QDebug& s, const Vec3<T>& v)
{
	s << "(" << v.x << "," << v.y << "," << v.z << ")";
	return s;
}

template<typename T>
QDataStream& operator <<(QDataStream& s, const Vec3<T>& v)
{
	s << v.x << v.y << v.z;
	return s;
}

template <typename T>
void insert(QMultiMap<float, Vec3<T> >& map, const Vec3<T>& point)
{
	float length = sqrt(pow(point.x, 2) + pow(point.y, 2) + pow(point.z, 2));
	
	map.insert(length, point);
}

template <typename T>
void insert(QMultiMap<float, Vec3<T> >& map, T x, T y, T z)
{
	insert(map, Vec3<T>(x, y, z));
}


typedef qint8 TypeName;

int main(int argc, char** argv)
{
	QCoreApplication app(argc, argv);
	
	if (argc < 3)
		qFatal("Usage: %s <extent> <output filename>", argv[0]);
	
	const int extent = QString(argv[1]).toInt();
	const QString outputFilename(argv[2]);
	
	QMultiMap<float, Vec3<TypeName> > map;
	for (TypeName x=-extent ; x<=extent ; ++x)
		for (TypeName y=-extent ; y<=extent ; ++y)
			for (TypeName z=-extent ; z<=extent ; ++z)
				insert(map, x, y, z);
	
	qDebug() << map.count() << "elements";
	
	QFile file(outputFilename);
	file.open(QIODevice::WriteOnly);
	QDataStream stream(&file);
	
	stream << (quint32) map.count();
	foreach (Vec3<TypeName> v, map)
		stream << v;
	
	return 0;
}
