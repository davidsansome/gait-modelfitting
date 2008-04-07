#ifndef FRAMESET_H
#define FRAMESET_H

#include <QStringList>
#include <QList>
#include <complex>

class FrameInfo;
class _Frame;

// TODO: Make this into a QAbstractItemModel?
class FrameSet
{
public:
	FrameSet(const QString& directory, bool loadInfoOnly = false);
	~FrameSet();
	
	void update();
	
	void updateSignature();
	void setClassification(const QString& classification);
	void clearMetadata();
	
	bool hasClassification() const { return !m_classification.isEmpty(); }
	QString classification() const { return m_classification; }
	QList<std::complex<double> > signature() const { return m_signature; }
	
	QString directory() const { return m_directory; }
	int count() const { return m_frames.count(); }
	QString name(int index) const;
	bool hasModelInformation(int index) const;
	FrameInfo* loadFrame(int index, bool loadInfoOnly = false);
	
	QStringList allNames() const;

private:
	FrameSet();
	
	void loadMetadata();
	void saveMetadata();
	
	QString m_directory;
	QList<_Frame> m_frames;
	
	QString m_classification;
	QList<std::complex<double> > m_signature;
};

#endif
