#ifndef LISTMODELS_H
#define LISTMODELS_H

#include "signature.h"

#include <QSortFilterProxyModel>
#include <QDirModel>
#include <complex>

class _Frame;
class _FrameSet;
class FrameInfo;

class FrameModel : public QDirModel
{
public:
	FrameModel(QObject* parent = 0);
	
	// To be used on frame sets
	bool hasClassification(const QModelIndex& index) const;
	QString classification(const QModelIndex& index) const;
	Signature signature(const QModelIndex& index) const;
	
	void setClassification(const QModelIndex& index, const QString& classification);
	void setSignature(const QModelIndex& index, const Signature& sig);
	void updateSignature(const QModelIndex& index);
	
	// To be used on frames
	bool hasModelInformation(const QModelIndex& index) const;
	FrameInfo* loadFrame(const QModelIndex& index, bool loadInfoOnly = false) const;
	
private:
	_Frame& frame(const QModelIndex& index) const;
	_FrameSet& frameSet(const QModelIndex& index) const;
	
	void saveFrameSet(const QModelIndex& index);
	
	QMap<QModelIndex, _Frame> m_frames;
	QMap<QModelIndex, _FrameSet> m_frameSets;
};


class FrameSetFilterModel : public QSortFilterProxyModel
{
public:
	FrameSetFilterModel(QObject* parent = 0);
	
	void setRootIndex(const QModelIndex& root) { m_rootIndex = root; }
	
	virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const;
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

protected:
	virtual bool filterAcceptsColumn(int sourceColumn, const QModelIndex& sourceParent) const;
	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;
	virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const;

private:
	QModelIndex m_rootIndex;
};


class FrameModelFilter : public QSortFilterProxyModel
{
public:
	FrameModelFilter(QObject* parent = 0);
	
	void setRootIndex(const QModelIndex& root) { m_rootIndex = root; }
	
protected:
	virtual bool filterAcceptsColumn(int sourceColumn, const QModelIndex& sourceParent) const;
	virtual bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const;
	virtual bool lessThan(const QModelIndex& left, const QModelIndex& right) const;
	
	QModelIndex m_rootIndex;
};

#endif
