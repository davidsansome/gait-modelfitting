#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "types.h"

class Voxel_Space;

class FrameInfo
{
public:
	FrameInfo(Voxel_Space* vspace);
	~FrameInfo();

	void analyse();
	
	const float2& center() const { return m_center; }
	int highestPoint() const { return m_highest; }
	int xWidth() const { return m_xWidth; }
	
	void bindTexture(int texUnit=0);
	const Voxel_Space* vspace() const { return m_vspace; }

private:
	Voxel_Space* m_vspace;
	
	int m_highest;
	float2 m_center;
	int m_xWidth;
	
	uint m_texture;
};

#endif
