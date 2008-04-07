#ifndef PEOPLE_H
#define PEOPLE_H

#include <QList>
#include <QStringList>

class FrameSet;

class People
{
public:
	static QStringList withClassifications();
	static void add(const QString& directory);

private:
	People();
	~People();
	
	static QStringList loadDirectories();
	static void saveDirectories(const QStringList& directories);
};

#endif
