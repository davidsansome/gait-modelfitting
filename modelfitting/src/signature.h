#ifndef SIGNATURE_H
#define SIGNATURE_H

#include <complex>
#include <QList>

class QSettings;

typedef QList<std::complex<double> > ComplexList;

class Signature
{
public:
	Signature() {}
	Signature(QSettings& s);
	
	void save(QSettings& s) const;
	void clear();
	double operator -(const Signature& other) const;
	
	ComplexList leftThighTheta;
	ComplexList rightThighTheta;

private:
	static ComplexList readList(QSettings& s, const QString& name);
	static void saveList(QSettings& s, const QString& name, const ComplexList& list);
	static double diff(const ComplexList& one, const ComplexList& two);
};

#endif
