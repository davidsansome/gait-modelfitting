#include "listmodels.h"
#include "frameinfo.h"
#include "fft.h"

#include <QDirModel>
#include <QtDebug>
#include <QSettings>

class _FrameSet
{
public:
	_FrameSet() {}
	_FrameSet(const QString& c) : classification(c) {}
	
	QString classification;
	Signature signature;
};

class _Frame
{
public:
	_Frame(bool i = false) : hasInfo(i) {}
	
	bool hasInfo;
};

FrameModel::FrameModel(QObject* parent)
	: QDirModel(parent)
{
}

_FrameSet& FrameModel::frameSet(const QModelIndex& index) const
{
	if (!m_frameSets.contains(index))
	{
		QString path(filePath(index) + QDir::separator() + "frameset.info");
		
		if (!QFile::exists(path))
			const_cast<FrameModel*>(this)->m_frameSets[index] = _FrameSet();
		else
		{
			QSettings s(path, QSettings::IniFormat);
			_FrameSet set(s.value("Classification", QVariant()).toString());
			
			int count = s.beginReadArray("Signature");
			for (int i=0 ; i<count ; ++i)
			{
				s.setArrayIndex(i);
				double real = s.value("Real", 0.0).toDouble();
				double imag = s.value("Imag", 0.0).toDouble();
				set.signature << std::complex<double>(real, imag);
			}
			s.endArray();
			
			const_cast<FrameModel*>(this)->m_frameSets[index] = set;
		}
	}
	return const_cast<FrameModel*>(this)->m_frameSets[index];
}

void FrameModel::saveFrameSet(const QModelIndex& index)
{
	_FrameSet set(frameSet(index));
	
	QString path(filePath(index) + QDir::separator() + "frameset.info");
	
	QSettings s(path, QSettings::IniFormat);
	s.setValue("Classification", set.classification);
	
	s.beginWriteArray("Signature", set.signature.count());
	for (int i=0 ; i<set.signature.count() ; ++i)
	{
		s.setArrayIndex(i);
		s.setValue("Real", set.signature[i].real());
		s.setValue("Imag", set.signature[i].imag());
	}
	s.endArray();
	
	emit dataChanged(index.sibling(index.row(), 0), index.sibling(index.row(), 1));
}

bool FrameModel::hasClassification(const QModelIndex& index) const
{
	return !frameSet(index).classification.isNull();
}

QString FrameModel::classification(const QModelIndex& index) const
{
	return frameSet(index).classification;
}

Signature FrameModel::signature(const QModelIndex& index) const
{
	return frameSet(index).signature;
}

void FrameModel::setClassification(const QModelIndex& index, const QString& classification)
{
	frameSet(index).classification = classification;
	saveFrameSet(index);
}

void FrameModel::setSignature(const QModelIndex& index, const Signature& sig)
{
	frameSet(index).signature = sig;
	saveFrameSet(index);
}

void FrameModel::updateSignature(const QModelIndex& index)
{
	Fft fft(this);
	fft.setFrameSet(index);
	fft.init();
	fft.run(Fft::LeftThighTheta);
	
	Signature& sig = frameSet(index).signature;
	sig.clear();
	for (int i=0 ; i<fft.resultSize() ; ++i)
		sig << fft.result()[i];
	
	saveFrameSet(index);
}

_Frame& FrameModel::frame(const QModelIndex& index) const
{
	if (!m_frames.contains(index))
	{
		QString path(filePath(index) + ".info");
		
		const_cast<FrameModel*>(this)->m_frames[index] = _Frame(QFile::exists(path));
	}
	return const_cast<FrameModel*>(this)->m_frames[index];
}

bool FrameModel::hasModelInformation(const QModelIndex& index) const
{
	return frame(index).hasInfo;
}

FrameInfo* FrameModel::loadFrame(const QModelIndex& index, bool loadInfoOnly) const
{
	if (!QFile::exists(filePath(index)))
		return NULL;
	
	return new FrameInfo(this, index, loadInfoOnly);
}




FrameSetFilterModel::FrameSetFilterModel(QObject* parent)
	: QSortFilterProxyModel(parent)
{
}

bool FrameSetFilterModel::hasChildren(const QModelIndex& parent) const
{
	if (!m_rootIndex.isValid())
		return QSortFilterProxyModel::hasChildren(parent);
	
	if (parent.parent() == m_rootIndex)
		return false;
	
	return true;
}

bool FrameSetFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
	QModelIndex row(sourceParent.child(sourceRow, 0));
	if (!row.isValid())
		return false;
	
	FrameModel* model = dynamic_cast<FrameModel*>(sourceModel());
	
	return model->fileInfo(row).isDir();
}

bool FrameSetFilterModel::filterAcceptsColumn(int sourceColumn, const QModelIndex& sourceParent) const
{
	return sourceColumn <= 1;
}

QVariant FrameSetFilterModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (section == 1)
		return "Classification";
	return QSortFilterProxyModel::headerData(section, orientation, role);
}

QVariant FrameSetFilterModel::data(const QModelIndex& index, int role) const
{
	FrameModel* model = dynamic_cast<FrameModel*>(sourceModel());
	
	if (index.column() == 1 && role == Qt::DisplayRole)
		return model->classification(mapToSource(index));
	return QSortFilterProxyModel::data(index, role);
}

bool FrameSetFilterModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
	bool leftOk, rightOk;
	int leftInt = left.data(Qt::DisplayRole).toInt(&leftOk);
	int rightInt = right.data(Qt::DisplayRole).toInt(&rightOk);
	if (!leftOk || !rightOk)
		return QSortFilterProxyModel::lessThan(left, right);
	
	return leftInt > rightInt;
}



FrameModelFilter::FrameModelFilter(QObject* parent)
	: QSortFilterProxyModel(parent)
{
}

bool FrameModelFilter::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
	QModelIndex row(sourceParent.child(sourceRow, 0));
	if (!row.isValid())
		return false;
	
	FrameModel* model = dynamic_cast<FrameModel*>(sourceModel());
	
	if (sourceParent != m_rootIndex)
		return true;
	
	QFileInfo info(model->fileInfo(row));
	return info.isFile() && info.fileName().endsWith(".Zspc");
}

bool FrameModelFilter::filterAcceptsColumn(int sourceColumn, const QModelIndex& sourceParent) const
{
	return sourceColumn <= 0;
}

bool FrameModelFilter::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
	return !QSortFilterProxyModel::lessThan(left, right);
}


