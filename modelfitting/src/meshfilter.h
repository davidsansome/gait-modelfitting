#ifndef MESHFILTER_H
#define MESHFILTER_H

#include <svl/SVL.h>
#include <QString>

class Model;
class FrameInfo;

class MeshFilter
{
public:
	MeshFilter(const QString& modelName, const QString& lookupTableName);
	~MeshFilter();
	
	void correlate(FrameInfo* info);

private:
	const Model* m_model;
	
	quint32 m_lookupElements;
	char* m_lookupData;
	Mat4 m_mat;
};

#endif
