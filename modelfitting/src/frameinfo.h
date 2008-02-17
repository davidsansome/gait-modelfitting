#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "types.h"

class Voxel_Space;
class Convolution;

#define MAXX 38.0
#define MINX (-MAXX)
#define MAXY 183.0
#define MINY (-MAXY)
#define MAXZ 100.0
#define MINZ (-MAXZ)

#define XEXTENT (MAXX*2)
#define YEXTENT (MAXY*2)
#define ZEXTENT (MAXZ*2)

class FrameInfo
{
public:
	FrameInfo(Voxel_Space* vspace);
	~FrameInfo();

	void analyse();
	void setFeatureVec(const Vec& fv) { m_featureVec = fv; }
	
	const float2& center() const { return m_center; }
	int highestPoint() const { return m_highest; }
	int xWidth() const { return m_xWidth; }
	Vec featureVec() const { return m_featureVec; }
	
	void bindTexture(int texUnit=0);
	const Voxel_Space* vspace() const { return m_vspace; }

private:
	Voxel_Space* m_vspace;
	
	int m_highest;
	float2 m_center;
	int m_xWidth;
	
	Vec m_featureVec;
	
	uint m_texture;
};

#endif
