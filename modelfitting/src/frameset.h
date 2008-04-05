#ifndef FRAMESET_H
#define FRAMESET_H

#include <QStringList>
#include <QList>

class FrameInfo;
class _Frame;

// TODO: Make this into a QAbstractItemModel?
class FrameSet
{
public:
	FrameSet(const QString& directory);
	~FrameSet();
	
	void update();
	
	QString directory() const { return m_directory; }
	int count() const { return m_frames.count(); }
	QString name(int index) const;
	bool hasModelInformation(int index) const;
	FrameInfo* loadFrame(int index, bool loadInfoOnly = false);
	
	QStringList allNames() const;

private:
	FrameSet();
	
	QString m_directory;
	QList<_Frame> m_frames;
};

#endif
