#include "signature.h"
#include "listmodels.h"
#include "fft.h"
#include "types.h"

#include <QSettings>
#include <QModelIndex>
#include <QtDebug>

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
	       diff(rightThighTheta, other.rightThighTheta)// +
	       /*diff(leftLowerTheta, other.leftLowerTheta) +
	       diff(rightLowerTheta, other.rightLowerTheta)*/;
}

void Signature::clear()
{
	leftThighTheta.clear();
	rightThighTheta.clear();
	leftLowerTheta.clear();
	rightLowerTheta.clear();
}

void Signature::normalizeMean(ComplexList& list)
{
	const int count = list.count();
	
	// Calculate the mean
	std::complex<double> mean = 0.0;
	for (int i=0 ; i<count ; ++i)
		mean += list[i];
	mean /= count;
	
	// Subtract the mean from all the samples
	for (int i=0 ; i<count ; ++i)
		list[i] -= mean;
}

void Signature::normalizeVariance(ComplexList& list)
{
	const int count = list.count();
	
	// Calculate the variance
	double real = 0.0;
	double imag = 0.0;
	for (int i=0 ; i<count ; ++i)
	{
		// I don't trust std::complex's pow function
		real += std::pow(list[i].real(), 2);
		imag += std::pow(list[i].imag(), 2);
	}
	real /= count;
	imag /= count;
	
	// Divide all the samples by the variance
	for (int i=0 ; i<count ; ++i)
		list[i] = std::complex<double>(list[i].real() / real, list[i].imag() / imag);
}

double Signature::diff(ComplexList one, ComplexList two)
{
	int count = qMin(one.count(), two.count());
	while (one.count() > count)
		one.removeLast();
	while (two.count() > count)
		two.removeLast();
	
	normalizeMean(one);
	normalizeMean(two);
	
	//normalizeVariance(one);
	//normalizeVariance(two);
	
	double accum = 0.0;
	for (int i=0 ; i<count ; ++i)
	{
		accum += std::pow(one[i].real() - two[i].real(), 2);
		accum += std::pow(one[i].imag() - two[i].imag(), 2);
	}
	
	return accum / count;
}

