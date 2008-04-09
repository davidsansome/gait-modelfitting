#include "people.h"
#include "listmodels.h"

#include <QSettings>

const FrameModel* People::s_model = NULL;

QStringList People::loadDirectories()
{
	QStringList ret;
	
	QSettings settings;
	int count = settings.beginReadArray("People");
	for (int i=0 ; i<count ; ++i)
	{
		settings.setArrayIndex(i);
		ret << settings.value("Directory").toString();
	}
	settings.endArray();
	
	return ret;
}

void People::saveDirectories(const QStringList& directories)
{
	QSettings settings;
	settings.beginWriteArray("People", directories.count());
	for (int i=0 ; i<directories.count() ; ++i)
	{
		settings.setArrayIndex(i);
		settings.setValue("Directory", directories[i]);
	}
	settings.endArray();
}

QStringList People::withClassifications()
{
	QStringList ret;
	QStringList directories(loadDirectories());
	
	foreach (QString dir, directories)
	{
		if (s_model->hasClassification(s_model->index(dir)))
			ret << dir;
	}
	
	return ret;
}

void People::add(const QString& directory)
{
	QStringList directories(loadDirectories());
	
	if (directories.contains(directory))
		return;
	
	saveDirectories(directories << directory);
}
