#ifndef FILTER_H
#define FILTER_H

#include <QString>

class Filter
{
public:
	Filter(const QString& fileName);
	~Filter();
	
	int xSize() const { return m_xSize; }
	int ySize() const { return m_ySize; }
	int zSize() const { return m_zSize; }
	
	float get(int x, int y, int z) const;
	const float* data() const { return m_data; }

private:
	quint8 m_xSize;
	quint8 m_ySize;
	quint8 m_zSize;
	
	float* m_data;
};

#endif
