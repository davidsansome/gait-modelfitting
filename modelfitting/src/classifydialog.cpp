#include "classifydialog.h"
#include "frameset.h"
#include "people.h"

typedef QPair<double, FrameSet*> Neighbour;

PersonItem::PersonItem(const QString& c, double d, QListWidget* parent)
	: QListWidgetItem(parent),
	  m_classification(c),
	  m_distance(d)
{
	setText(c + " (distance " + QString::number(d) + ")");
}

ClassifyDialog::ClassifyDialog(QWidget* parent)
	: QDialog(parent),
	  m_frameSet(NULL)
{
	m_ui.setupUi(this);
	
	connect(m_ui.buttonBox, SIGNAL(accepted()), SLOT(okClicked()));
}

ClassifyDialog::~ClassifyDialog()
{
}

void ClassifyDialog::setFrameSet(FrameSet* frameSet)
{
	m_frameSet = frameSet;
	m_ui.seenBeforeList->clear();
	m_ui.warning->hide();
	
	QList<Neighbour> neighbours;
	QList<FrameSet*> sets;
	
	QStringList people(People::withClassifications());
	foreach (QString dir, people)
	{
		FrameSet* set = new FrameSet(dir, true);
		sets << set;
		
		if (dir == m_frameSet->directory())
		{
			m_ui.warning->setText("<p align=center>You've already classified this person as <b>" + set->classification() + "</b>.<br>" +
				"Entering a new classification will overwrite this existing one.</p>");
			m_ui.warning->show();
		}
		else
			neighbours << Neighbour(distanceTo(set), set);
	}
	
	qSort(neighbours);
	foreach (Neighbour neighbour, neighbours)
		new PersonItem(neighbour.second->classification(), neighbour.first, m_ui.seenBeforeList);
	
	qDeleteAll(sets);
	
	m_ui.seenBefore->setDisabled(neighbours.count() == 0);
}

void ClassifyDialog::okClicked()
{
	if (m_ui.someoneNew->isChecked())
	{
		m_frameSet->setClassification(m_ui.newName->text());
	}
	else
	{
		PersonItem* person = dynamic_cast<PersonItem*>(m_ui.seenBeforeList->currentItem());
		m_frameSet->setClassification(person->classification());
	}
	
	People::add(m_frameSet->directory());
	accept();
}

double ClassifyDialog::distanceTo(FrameSet* other) const
{
	double accum = 0.0;
	
	int count = qMin(other->signature().count(), m_frameSet->signature().count());
	for (int i=0 ; i<count ; ++i)
	{
		double ourPhaseMag = abs(m_frameSet->signature()[i]) * arg(m_frameSet->signature()[i]);
		double theirPhaseMag = abs(other->signature()[i]) * arg(other->signature()[i]);
		
		accum += std::abs(ourPhaseMag - theirPhaseMag);
	}
	
	return accum;
}
