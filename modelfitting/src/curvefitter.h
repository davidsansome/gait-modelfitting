#ifndef CURVEFITTER_H
#define CURVEFITTER_H

#include <QtDebug>
#include <QModelIndex>

class FrameModel;
class FrameModelFilter;

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
	CurveFitter(FrameModel* model);
	~CurveFitter();
	
	void setFrameSet(const QModelIndex& frameSet);
	QModelIndex frameSet() const { return m_index; }
	
	FittingResult doFitting(int min = -1, int max = -1);

private:
	void initData(int min, int max);
	float energy(int min, float period, float phase);
	
	QModelIndex m_index;
	FrameModel* m_model;
	FrameModelFilter* m_filter;
	
	int m_dataSize;
	float* m_data;
	float* m_dataEnd;
};

#endif
