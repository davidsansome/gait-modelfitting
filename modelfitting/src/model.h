#ifndef MODEL_H
#define MODEL_H

#include <svl/SVL.h>
#include <QString>

class Model
{
public:
	Model(const QString& name);
	
	void setVertices(int count, const Vec3* data);
	
	QString name() const { return m_name; }
	int vertexCount() const { return m_vertexCount; }
	const Vec3* vertexData() const { return m_vertexData; }

private:
	QString m_name;
	int m_vertexCount;
	const Vec3* m_vertexData;
};

#endif
