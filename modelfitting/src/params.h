#ifndef PARAMS_H
#define PARAMS_H

#include <QSettings>
#include <QVariant>
#include <QtDebug>

template <typename T>
class Params
{
public:
	Params();
	Params(T ta, T tt, T la, T lt);
	Params(const Params& other);
	Params(QSettings& settings);
	
	bool operator <(const Params& other) const;
	Params& operator =(const Params& other);
	
	void invalidate() { valid = false; }
	
	void save(QSettings& settings) const;
	
	bool valid;
	T thighAlpha;
	T thighTheta;
	T lowerLegAlpha;
	T lowerLegTheta;
};

template <typename T>
QDebug operator <<(QDebug s, const Params<T>& p)
{
	s.nospace() << "Params(" << p.thighAlpha << ", " << p.thighTheta << ", " << p.lowerLegAlpha << ", " << p.lowerLegTheta << ")";
	return s.space();
}

template <typename T>
Params<T>::Params()
	: valid(false),
	  thighAlpha(0),
	  thighTheta(0),
	  lowerLegAlpha(0),
	  lowerLegTheta(0)
{
}

template <typename T>
Params<T>::Params(T ta, T tt, T la, T lt)
	: valid(true),
	  thighAlpha(ta),
	  thighTheta(tt),
	  lowerLegAlpha(la),
	  lowerLegTheta(lt)
{
}

template <typename T>
Params<T>::Params(const Params& other)
{
	*this = other;
}

#ifndef Q_WS_X11

// This is specialized in params.cpp for Windows and mac, because they suck

template <typename T>
Params<T>::Params(QSettings& settings)
	: valid(true)
{
	thighAlpha = settings.value("ThighAlpha", 0.0).value<T>();
	thighTheta = settings.value("ThighTheta", 0.0).value<T>();
	lowerLegAlpha = settings.value("LowerLegAlpha", 0.0).value<T>();
	lowerLegTheta = settings.value("LowerLegTheta", 0.0).value<T>();
}

#endif

template <typename T>
Params<T>& Params<T>::operator =(const Params& other)
{
	valid = other.valid;
	thighAlpha = other.thighAlpha;
	thighTheta = other.thighTheta;
	lowerLegAlpha = other.lowerLegAlpha;
	lowerLegTheta = other.lowerLegTheta;
	return *this;
}

template <typename T>
bool Params<T>::operator <(const Params& other) const
{
	if (thighAlpha != other.thighAlpha)
		return thighAlpha < other.thighAlpha;
	if (thighTheta != other.thighTheta)
		return thighTheta < other.thighTheta;
	if (lowerLegAlpha != other.lowerLegAlpha)
		return lowerLegAlpha < other.lowerLegAlpha;
	if (lowerLegTheta != other.lowerLegTheta)
		return lowerLegTheta < other.lowerLegTheta;
	return false;
}

template <typename T>
void Params<T>::save(QSettings& settings) const
{
	settings.setValue("ThighAlpha", thighAlpha);
	settings.setValue("ThighTheta", thighTheta);
	settings.setValue("LowerLegAlpha", lowerLegAlpha);
	settings.setValue("LowerLegTheta", lowerLegTheta);
}

#endif
