#include "classifydialog.h"
#include "listmodels.h"
#include "people.h"

typedef QPair<double, QModelIndex> Neighbour;

PersonItem::PersonItem(const QString& c, double d, QListWidget* parent)
	: QListWidgetItem(parent),
	  m_classification(c),
	  m_distance(d)
{
	setText(c + " (distance " + QString::number(d) + ")");
}

ClassifyDialog::ClassifyDialog(FrameModel* model, QWidget* parent)
	: QDialog(parent),
	  m_model(model)
{
	m_ui.setupUi(this);
	
	connect(m_ui.buttonBox, SIGNAL(accepted()), SLOT(okClicked()));
}

ClassifyDialog::~ClassifyDialog()
{
}

void ClassifyDialog::setFrameSet(const QModelIndex& frameSet)
{
	m_index = frameSet;
	
	m_ui.seenBeforeList->clear();
	m_ui.warning->hide();
	
	QList<Neighbour> neighbours;
	QList<QModelIndex> sets;
	
	QStringList people(People::withClassifications());
	foreach (QString dir, people)
	{
		QModelIndex set(m_model->index(dir));
		sets << set;
		
		if (set == m_index)
		{
			m_ui.warning->setText("<p align=center>You've already classified this person as <b>" + m_model->classification(set) + "</b>.<br>" +
				"Entering a new classification will overwrite this existing one.</p>");
			m_ui.warning->show();
		}
		else
			neighbours << Neighbour(distanceTo(set), set);
	}
	
	qSort(neighbours);
	foreach (Neighbour neighbour, neighbours)
		new PersonItem(m_model->classification(neighbour.second), neighbour.first, m_ui.seenBeforeList);
	
	m_ui.seenBefore->setDisabled(neighbours.count() == 0);
	if (neighbours.count() == 0)
		m_ui.someoneNew->setChecked(true);
	
	if (m_ui.someoneNew->isChecked())
		m_ui.newName->setFocus(Qt::OtherFocusReason);
	else
		m_ui.seenBeforeList->setFocus(Qt::OtherFocusReason);
}

void ClassifyDialog::okClicked()
{
	if (m_ui.someoneNew->isChecked())
	{
		m_model->setClassification(m_index, m_ui.newName->text());
	}
	else
	{
		PersonItem* person = dynamic_cast<PersonItem*>(m_ui.seenBeforeList->currentItem());
		m_model->setClassification(m_index, person->classification());
	}
	
	People::add(m_model->filePath(m_index));
	accept();
}

double ClassifyDialog::distanceTo(const QModelIndex& other) const
{
	double accum = 0.0;
	
	int count = qMin(m_model->signature(other).count(), m_model->signature(m_index).count());
	for (int i=0 ; i<count ; ++i)
	{
		double ourPhaseMag = abs(m_model->signature(m_index)[i]) * arg(m_model->signature(m_index)[i]);
		double theirPhaseMag = abs(m_model->signature(other)[i]) * arg(m_model->signature(other)[i]);
		
		accum += std::pow(ourPhaseMag - theirPhaseMag, 2);
		// TODO: Normalize variance and mean
		// Canonical analysis ??
		// Include other leg as well.  And lower leg.
	}
	
	return accum;
}
