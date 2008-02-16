#ifndef MODEL_H
#define MODEL_H

#include <svl/SVL.h>
#include <QString>

class Model
{
public:
	Model(const QString& name);
	
	void setVertices(int count, const Vec4* data);
	void setMinMax(const Vec3& min, const Vec3& max);
	
	QString name() const { return m_name; }
	int vertexCount() const { return m_vertexCount; }
	const Vec4* vertexData() const { return m_vertexData; }
	Vec3 min() const { return m_min; }
	Vec3 max() const { return m_max; }
	Vec3 extent() const { return m_max - m_min; }

private:
	QString m_name;
	int m_vertexCount;
	const Vec4* m_vertexData;
	
	Vec3 m_min;
	Vec3 m_max;
};

#endif
