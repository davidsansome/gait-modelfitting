#include "params.h"

#ifdef Q_WS_WIN

// These constructors are specalized because QVariant::value<T> doesn't seem to work on Windows...

template <>
Params<float>::Params(QSettings& settings)
	: valid(true)
{
	thighAlpha = settings.value("ThighAlpha", 0.0).toDouble();
	thighTheta = settings.value("ThighTheta", 0.0).toDouble();
	lowerLegAlpha = settings.value("LowerLegAlpha", 0.0).toDouble();
	lowerLegTheta = settings.value("LowerLegTheta", 0.0).toDouble();
}

template <>
Params<int>::Params(QSettings& settings)
	: valid(true)
{
	thighAlpha = settings.value("ThighAlpha", 0.0).toInt();
	thighTheta = settings.value("ThighTheta", 0.0).toInt();
	lowerLegAlpha = settings.value("LowerLegAlpha", 0.0).toInt();
	lowerLegTheta = settings.value("LowerLegTheta", 0.0).toInt();
}

#endif