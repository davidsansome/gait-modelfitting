#ifndef CURVEFITTER_H
#define CURVEFITTER_H

#include <QtDebug>

class FrameSet;

class FittingResult
{
public:
	FittingResult(float dc = 0.0, float a = 0.0, float pe = 0.0, float ph = 0.0, float e = std::numeric_limits<float>::max())
		: dcOffset(dc), amplitude(a), period(pe), phase(ph), energy(e) {}
	
	bool operator >(const FittingResult& other) const { return energy > other.energy; }
	bool operator >=(const FittingResult& other) const { return energy >= other.energy; }
	bool operator ==(const FittingResult& other) const { return energy == other.energy; }
	bool operator <(const FittingResult& other) const { return energy < other.energy; }
	bool operator <=(const FittingResult& other) const { return energy <= other.energy; }
	
	void calculateEnergy(int min, const float* data, const float* dataEnd);
	
	float zeroCrossing() const;
	
	float dcOffset;
	float amplitude;
	
	float period;
	float phase;
	
	float energy;
};

QDebug& operator <<(QDebug& s, const FittingResult& f);


class CurveFitter
{
public:
	CurveFitter();
	~CurveFitter();
	
	void setFrameSet(FrameSet* frameSet) { m_frameSet = frameSet; }
	FrameSet* frameSet() const { return m_frameSet; }
	
	FittingResult doFitting(int min = -1, int max = -1);

private:
	void initData(int min, int max);
	float energy(int min, float period, float phase);
	
	FrameSet* m_frameSet;
	int m_dataSize;
	float* m_data;
	float* m_dataEnd;
};

#endif
