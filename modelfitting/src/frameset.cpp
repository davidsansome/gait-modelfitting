#include "frameset.h"
#include "frameinfo.h"
#include "fft.h"

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

FrameSet::FrameSet(const QString& directory, bool loadInfoOnly)
	: m_directory(directory)
{
	if (!loadInfoOnly)
		update();
	
	loadMetadata();
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
	QString fileName(m_directory + QDir::separator() + name(index));
	
	if (!QFile::exists(fileName))
		return NULL;
	
	return new FrameInfo(this, fileName, loadInfoOnly);
}

QStringList FrameSet::allNames() const
{
	QStringList ret;
	foreach (_Frame frame, m_frames)
		ret << frame.name;
	return ret;
}

void FrameSet::loadMetadata()
{
	QSettings s(m_directory + QDir::separator() + "frameset.info", QSettings::IniFormat);
	m_classification = s.value("Classification", QVariant()).toString();
	
	m_signature.clear();
	int count = s.beginReadArray("Signature");
	for (int i=0 ; i<count ; ++i)
	{
		s.setArrayIndex(i);
		double real = s.value("Real", 0.0).toDouble();
		double imag = s.value("Imag", 0.0).toDouble();
		m_signature << std::complex<double>(real, imag);
	}
	s.endArray();
}

void FrameSet::saveMetadata()
{
	QSettings s(m_directory + QDir::separator() + "frameset.info", QSettings::IniFormat);
	s.setValue("Classification", m_classification);
	
	s.beginWriteArray("Signature", m_signature.count());
	for (int i=0 ; i<m_signature.count() ; ++i)
	{
		s.setArrayIndex(i);
		s.setValue("Real", m_signature[i].real());
		s.setValue("Imag", m_signature[i].imag());
	}
	s.endArray();
}

void FrameSet::clearMetadata()
{
	QFile::remove(m_directory + QDir::separator() + "frameset.info");
	m_classification = QString::null;
	m_signature.clear();
}

void FrameSet::setClassification(const QString& classification)
{
	m_classification = classification;
	saveMetadata();
}

void FrameSet::updateSignature()
{
	Fft fft;
	fft.setFrameSet(this);
	fft.init();
	fft.run(Fft::LeftThighTheta);
	
	m_signature.clear();
	for (int i=0 ; i<fft.resultSize() ; ++i)
		m_signature << fft.result()[i];
	
	saveMetadata();
}

