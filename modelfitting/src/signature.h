#ifndef SIGNATURE_H
#define SIGNATURE_H

#include <complex>
#include <QList>

class QSettings;
class QModelIndex;
class FrameModel;

typedef QList<std::complex<double> > ComplexList;

class Signature
{
public:
	Signature() {}
	Signature(QSettings& s);
	
	void update(FrameModel* model, const QModelIndex& index);
	
	void save(QSettings& s) const;
	void clear();
	double operator -(const Signature& other) const;
	
	float height;
	ComplexList leftThighTheta;
	ComplexList rightThighTheta;
	ComplexList leftLowerTheta;
	ComplexList rightLowerTheta;

private:
	void init();
	
	static ComplexList readList(QSettings& s, const QString& name);
	static void saveList(QSettings& s, const QString& name, const ComplexList& list);
	static double diff(ComplexList one, ComplexList two);
	
	static void normalizeVariance(ComplexList& list);
	static void normalizeMean(ComplexList& list);
};

#endif
