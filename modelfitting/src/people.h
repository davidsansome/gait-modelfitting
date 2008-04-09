#ifndef PEOPLE_H
#define PEOPLE_H

#include <QList>
#include <QStringList>

class FrameModel;

class People
{
public:
	static void setModel(const FrameModel* model) { s_model = model; }
	
	static QStringList withClassifications();
	static void add(const QString& directory);

private:
	People();
	~People();
	
	static QStringList loadDirectories();
	static void saveDirectories(const QStringList& directories);
	
	static const FrameModel* s_model;
};

#endif
