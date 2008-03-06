#include "frameset.h"
#include "frameinfo.h"

#include <QDir>

class _Frame
{
public:
	QString name;
	bool hasModelInformation;
};


FrameSet::FrameSet()
{
	qFatal("Don't call the default constructor of FrameSet\n");
}

FrameSet::FrameSet(const QString& directory)
	: m_directory(directory)
{
	update();
}

FrameSet::~FrameSet()
{
}

void FrameSet::update()
{
	m_frames.clear();
	
	QDir dir(m_directory);
	QStringList rows(dir.entryList(QStringList() << "*.Zspc", QDir::Files | QDir::NoDotAndDotDot, QDir::Name));
	foreach (QString row, rows)
	{
		_Frame frame;
		frame.name = row;
		frame.hasModelInformation = QFile::exists(m_directory + QDir::separator() + row + ".info");
		m_frames << frame;
	}
}

QString FrameSet::name(int index) const
{
	return m_frames[index].name;
}

bool FrameSet::hasModelInformation(int index) const
{
	return m_frames[index].hasModelInformation;
}

FrameInfo* FrameSet::loadFrame(int index, bool loadInfoOnly)
{
	return new FrameInfo(this, m_directory + QDir::separator() + name(index), loadInfoOnly);
}

QStringList FrameSet::allNames() const
{
	QStringList ret;
	foreach (_Frame frame, m_frames)
		ret << frame.name;
	return ret;
}

