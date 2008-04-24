#include <QApplication>
#include <QtDebug>
#include <QFile>
#include <QList>
#include <QIODevice>
#include <QStringList>

#include <iostream>
#include <cmath>
#include <sstream>
#include <iomanip>

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
	qDebug() << "Read" << leftThigh.count() << "values";
}

float compare(const QList<float>& a, const QList<float>& m)
{
	Q_ASSERT(a.count() == m.count());
	
	float diff = 0.0;
	//float diffSquared = 0.0;
	
	for (int i=0 ; i<a.count() ; ++i)
	{
		float d = qAbs(a[i] - m[i]);
		
		diff += d;
		//diffSquared += pow(d, 2);
	}
	
	//cout << qPrintable(label) << " & " << diff << " & " << diff/a.count() << " & " << diffSquared << " & " << diffSquared/a.count() << " \\\\" << endl;
	
	//diffTot += diff;
	//diffSquaredTot += diffSquared;
	
	return diff / a.count();
}

QList<float> doSample(const QString& autoFilename, const QString& manualFilename)
{
	qDebug() << "Doing" << autoFilename << manualFilename;
	
	QFile autoFile(autoFilename);
	QFile manualFile(manualFilename);
	
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
	
	Q_ASSERT(autoFile.open(QIODevice::ReadOnly));
	Q_ASSERT(manualFile.open(QIODevice::ReadOnly));
	
	readFile(autoFile, autoLeftThigh, autoLeftLower, autoRightThigh, autoRightLower);
	readFile(manualFile, manualLeftThigh, manualLeftLower, manualRightThigh, manualRightLower);
	
	// Compare the values
	QList<float> ret;
	
	ret << compare(autoLeftThigh, manualLeftThigh);
	ret << compare(autoRightThigh, manualRightThigh);
	ret << compare(autoLeftLower, manualLeftLower);
	ret << compare(autoRightLower, manualRightLower);
	ret << (ret[0] + ret[1] + ret[2] + ret[3]) / 4.0;
	
	return ret;
}

string toPercent(float in)
{
	const float totalRange = M_PI / 2.0;
	
	ostringstream os;
	os << fixed << setprecision(2) << (in / totalRange) * 100.0;
	
	return os.str() + "\\%";
}

int main(int argc, char** argv)
{
	QApplication app(argc, argv, false);
	
	QList<QList<float> > table;
	
	table << doSample("set81-res10.dat", "set81-manual.dat");
	table << doSample("set96-res10.dat", "set96-manual.dat");
	table << doSample("set81-res20.dat", "set81-manual.dat");
	table << doSample("set96-res20.dat", "set96-manual.dat");
	table << doSample("set81-multires.dat", "set81-manual.dat");
	table << doSample("set96-multires.dat", "set96-manual.dat");
	
	cout << "Left thigh & " <<
		toPercent(table[0][0]) << " & " << toPercent(table[1][0]) << " & " <<
		toPercent(table[2][0]) << " & " << toPercent(table[3][0]) << " & " <<
		toPercent(table[4][0]) << " & " << toPercent(table[5][0]) << " \\\\" << endl;
	
	cout << "Right thigh & " <<
		toPercent(table[0][1]) << " & " << toPercent(table[1][1]) << " & " <<
		toPercent(table[2][1]) << " & " << toPercent(table[3][1]) << " & " <<
		toPercent(table[4][1]) << " & " << toPercent(table[5][1]) << " \\\\" << endl;
	
	cout << "Left lower leg & " <<
		toPercent(table[0][2]) << " & " << toPercent(table[1][2]) << " & " <<
		toPercent(table[2][2]) << " & " << toPercent(table[3][2]) << " & " <<
		toPercent(table[4][2]) << " & " << toPercent(table[5][2]) << " \\\\" << endl;
	
	cout << "Right lower leg & " << toPercent(table[0][3]) << " & " <<
		toPercent(table[1][3]) << " & " << toPercent(table[2][3]) << " & " <<
		toPercent(table[3][3]) << " & " << toPercent(table[4][3]) << " & " <<
		toPercent(table[5][3]) << " \\\\" << endl;
	
	cout << "\\hline" << endl;
	cout << "Overall & " <<
		toPercent(table[0][4]) << " & " << toPercent(table[1][4]) << " & " <<
		toPercent(table[2][4]) << " & " << toPercent(table[3][4]) << " & " <<
		toPercent(table[4][4]) << " & " << toPercent(table[5][4]) << " \\\\" << endl;
	
	cout << "& " <<
		"\\multicolumn{2}{|c|}{" << toPercent((table[0][4] + table[1][4]) / 2.0) << "} & " <<
		"\\multicolumn{2}{|c|}{" << toPercent((table[2][4] + table[3][4]) / 2.0) << "} & " <<
		"\\multicolumn{2}{|c}{" << toPercent((table[4][4] + table[5][4]) / 2.0) << "} \\\\" << endl;
}

