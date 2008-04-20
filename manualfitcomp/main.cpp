#include <QApplication>
#include <QtDebug>
#include <QFile>
#include <QList>
#include <QIODevice>
#include <QStringList>

#include <iostream>
#include <cmath>

using namespace std;


void readFile(QIODevice& file, QList<float>& leftThigh, QList<float>& leftLower, QList<float>& rightThigh, QList<float>& rightLower)
{
	while (!file.atEnd())
	{
		QString line(file.readLine());
		line = line.trimmed();
		
		if (line.isEmpty())
			break;
		
		QStringList values(line.split(" "));
		Q_ASSERT(values.size() == 9);
		
		bool ok;
		leftThigh << values[2].toFloat(&ok);    Q_ASSERT(ok == true);
		leftLower << values[4].toFloat(&ok);    Q_ASSERT(ok == true);
		rightThigh << values[6].toFloat(&ok);   Q_ASSERT(ok == true);
		rightLower << values[8].toFloat(&ok);   Q_ASSERT(ok == true);
	}
}

void compare(const QString& label, const QList<float>& a, const QList<float>& m, float& diffTot, float& diffSquaredTot)
{
	Q_ASSERT(a.count() == m.count());
	
	float diff = 0.0;
	float diffSquared = 0.0;
	
	for (int i=0 ; i<a.count() ; ++i)
	{
		float d = qAbs(a[i] - m[i]);
		
		diff += d;
		diffSquared += pow(d, 2);
	}
	
	cout << qPrintable(label) << " & " << diff << " & " << diff/a.count() << " & " << diffSquared << " & " << diffSquared/a.count() << " \\\\" << endl;
	
	diffTot += diff;
	diffSquaredTot += diffSquared;
}

int main(int argc, char** argv)
{
	QApplication app(argc, argv, false);
	
	// Check our arguments
	if (argc != 3)
		qFatal("Usage: %s autofit.dat manualfit.dat\n", argv[0]);
	
	QFile autoFile(argv[1]);
	QFile manualFile(argv[2]);
	
	if (!autoFile.exists())
		qFatal("%s does not exist\n", qPrintable(autoFile.fileName()));
	
	if (!manualFile.exists())
		qFatal("%s does not exist\n", qPrintable(manualFile.fileName()));
	
	
	// Read the files
	QList<float> autoLeftThigh;
	QList<float> autoLeftLower;
	QList<float> autoRightThigh;
	QList<float> autoRightLower;
	QList<float> manualLeftThigh;
	QList<float> manualLeftLower;
	QList<float> manualRightThigh;
	QList<float> manualRightLower;
	
	autoFile.open(QIODevice::ReadOnly);
	manualFile.open(QIODevice::ReadOnly);
	
	readFile(autoFile, autoLeftThigh, autoLeftLower, autoRightThigh, autoRightLower);
	readFile(manualFile, manualLeftThigh, manualLeftLower, manualRightThigh, manualRightLower);
	
	// Compare the values
	float diff = 0.0;
	float diffSquared = 0.0;
	
	compare("Left thigh", autoLeftThigh, manualLeftThigh, diff, diffSquared);
	compare("Right thigh", autoRightThigh, manualRightThigh, diff, diffSquared);
	compare("Left lower", autoLeftLower, manualLeftLower, diff, diffSquared);
	compare("Right lower", autoRightLower, manualRightLower, diff, diffSquared);
	
	cout << "\\hline" << endl;
	cout << "Total & " << diff << " & " << diff/autoLeftThigh.count() << " & " << diffSquared << " & " << diffSquared/autoLeftThigh.count() << " \\\\" << endl;
	
	return 0;
}

