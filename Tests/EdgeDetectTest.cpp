#include <QDataStream>
#include <QTextStream>
#include "FindContours.h"
#include <QApplication>
#include <QDebug>
#include <QFile>

using namespace std;

void saveCSVFromImage(string imgDir, string csvFile);

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	string imgDir = "C:/Database/qubodup-Cog-cogwheel-gear-Zahnrad-6.png";
	string csvFile = "results.csv";
	saveCSVFromImage(imgDir, csvFile);
	return 1;
}

void saveCSVFromImage(string imgDir, string csvFile)
{
	vector<vector<cv::Point> > outerContours;
	vector<vector<vector<cv::Point> > > innerContours;

	bool savefortesting = true;
	gbFindContours(imgDir, outerContours, innerContours, savefortesting);

	QFile file(QString::fromStdString(csvFile));
	file.open(QIODevice::WriteOnly | QIODevice::Text);
	QTextStream out(&file);

	out << "X_coords, Y_coords\n";

	qDebug() << "Outer contours" << outerContours.size();
	qDebug() << "Inner contours" << innerContours.size();

	int num_outer_loops = outerContours.size();
	for (size_t i=0; i < num_outer_loops; i++)
	{
		vector<cv::Point> singleLoop = outerContours[i];
		qDebug() << singleLoop.size();
		for (int j=0; j < singleLoop.size(); j++)
		{
			cv::Point pt = singleLoop[j];
			out << QString::number(pt.x) <<"," << QString::number(pt.y) << "\n";
		}
	}

	int num_inner_loops = innerContours.size();
	for (size_t i=0; i < num_outer_loops; i++)
	{
		vector<vector<cv::Point> > multiOuters = innerContours[i];
		for (int j=0; j < multiOuters.size(); j++)
		{
			vector<cv::Point> f_innerLoop = multiOuters[j];
			for (int k=0; k< f_innerLoop.size(); k++)
			{
				cv::Point pt = f_innerLoop[k];
				out << QString::number(pt.x) <<"," << QString::number(pt.y) << "\n";
//				qDebug() << pt.x <<"," << pt.y ;
			}
		}
	}

	file.close();
	qDebug() << "Everything done!";
}
