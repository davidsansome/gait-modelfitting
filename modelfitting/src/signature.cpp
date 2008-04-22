#include "signature.h"
#include "listmodels.h"
#include "fft.h"

#include <QSettings>
#include <QModelIndex>

Signature::Signature(QSettings& s)
{
	s.beginGroup("Signature");
	
	leftThighTheta = readList(s, "LeftThighTheta");
	rightThighTheta = readList(s, "RightThighTheta");
	leftLowerTheta = readList(s, "LeftLowerTheta");
	rightLowerTheta = readList(s, "RightLowerTheta");
	
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

void Signature::update(FrameModel* model, const QModelIndex& index)
{
	clear();
	
	Fft fft(model);
	fft.setFrameSet(index);
	fft.init();
	
	fft.run(Fft::LeftThighTheta);
	for (int i=0 ; i<fft.resultSize() ; ++i)
		leftThighTheta << fft.result()[i];
	
	fft.run(Fft::RightThighTheta);
	for (int i=0 ; i<fft.resultSize() ; ++i)
		rightThighTheta << fft.result()[i];
	
	fft.run(Fft::LeftLowerTheta);
	for (int i=0 ; i<fft.resultSize() ; ++i)
		leftLowerTheta << fft.result()[i];
	
	fft.run(Fft::RightLowerTheta);
	for (int i=0 ; i<fft.resultSize() ; ++i)
		rightLowerTheta << fft.result()[i];
}

void Signature::save(QSettings& s) const
{
	s.beginGroup("Signature");
	
	saveList(s, "LeftThighTheta", leftThighTheta);
	saveList(s, "RightThighTheta", rightThighTheta);
	saveList(s, "LeftLowerTheta", leftLowerTheta);
	saveList(s, "RightLowerTheta", rightLowerTheta);
	
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
	       diff(rightThighTheta, other.rightThighTheta)/* +
	       diff(leftLowerTheta, other.leftLowerTheta) +
	       diff(rightLowerTheta, other.rightLowerTheta)*/;
}

void Signature::clear()
{
	leftThighTheta.clear();
	rightThighTheta.clear();
	leftLowerTheta.clear();
	rightLowerTheta.clear();
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

