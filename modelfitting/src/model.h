#ifndef MODEL_H
#define MODEL_H

#include <svl/SVL.h>
#include <QString>

class Vertex
{
public:
	Vertex(const Vec4& p, int m) : pos(p), mat(m) {}
	Vec4 pos;
	int mat;
};

class Material
{
public:
	Material(const Vec3& c) : color(c) {}
	Vec3 color;
};

class Triangle
{
public:
	Triangle(int ve1, int ve2, int ve3, int m) : mat(m), v1(ve1), v2(ve2), v3(ve3) {}
	
	int mat;
	int v1;
	int v2;
	int v3;
};

class Model
{
public:
	Model(const QString& name);
	
	void drawPoints() const;
	void draw() const;
	
	void setVertices(int count, const Vertex* data);
	void setMaterials(int count, const Material* data);
	void setTriangles(int count, const Triangle* data);
	void setMinMax(const Vec3& min, const Vec3& max);
	
	QString name() const { return m_name; }
	
	int vertexCount() const { return m_vertexCount; }
	const Vertex* vertexData() const { return m_vertexData; }
	
	int materialCount() const { return m_materialCount; }
	const Material* materialData() const { return m_materialData; }
	
	int triangleCount() const { return m_triangleCount; }
	const Triangle* triangleData() const { return m_triangleData; }
	
	Vec3 min() const { return m_min; }
	Vec3 max() const { return m_max; }
	Vec3 extent() const { return m_max - m_min; }

private:
	QString m_name;
	int m_vertexCount;
	const Vertex* m_vertexData;
	int m_materialCount;
	const Material* m_materialData;
	int m_triangleCount;
	const Triangle* m_triangleData;
	
	Vec3 m_min;
	Vec3 m_max;
};

#endif
