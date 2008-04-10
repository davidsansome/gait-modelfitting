#include "signature.h"

#include <QSettings>

Signature::Signature(QSettings& s)
{
	s.beginGroup("Signature");
	
	leftThighTheta = readList(s, "LeftThighTheta");
	rightThighTheta = readList(s, "RightThighTheta");
	
	s.endGroup();
}

ComplexList Signature::readList(QSettings& s, const QString& name)
{
	ComplexList ret;
	
	int count = s.beginReadArray(name);
	for (int i=0 ; i<count ; ++i)
	{
		s.setArrayIndex(i);
		double real = s.value("Real", 0.0).toDouble();
		double imag = s.value("Imag", 0.0).toDouble();
		ret << std::complex<double>(real, imag);
	}
	s.endArray();
	
	return ret;
}

void Signature::save(QSettings& s) const
{
	s.beginGroup("Signature");
	
	saveList(s, "LeftThighTheta", leftThighTheta);
	saveList(s, "RightThighTheta", rightThighTheta);
	
	s.endGroup();
}

void Signature::saveList(QSettings& s, const QString& name, const ComplexList& list)
{
	s.beginWriteArray(name, list.count());
	for (int i=0 ; i<list.count() ; ++i)
	{
		s.setArrayIndex(i);
		s.setValue("Real", list[i].real());
		s.setValue("Imag", list[i].imag());
	}
	s.endArray();
}

double Signature::operator -(const Signature& other) const
{
	return diff(leftThighTheta, other.leftThighTheta) +
	       diff(rightThighTheta, other.rightThighTheta);
}

void Signature::clear()
{
	leftThighTheta.clear();
	rightThighTheta.clear();
}

double Signature::diff(const ComplexList& one, const ComplexList& two)
{
	int count = qMin(one.count(), two.count());
	
	std::complex<double> meanOne = 0.0;
	std::complex<double> meanTwo = 0.0;
	for (int i=0 ; i<count ; ++i)
	{
		meanOne += one[i];
		meanTwo += two[i];
	}
	meanOne /= count;
	meanTwo /= count;
	
	double accum = 0.0;
	for (int i=0 ; i<count ; ++i)
	{
		std::complex<double> dOne = one[i] - meanOne;
		std::complex<double> dTwo = two[i] - meanTwo;
		
		accum += std::pow(dOne.real() - dTwo.real(), 2);
		accum += std::pow(dOne.imag() - dTwo.imag(), 2);
	}
	
	return accum;
}

