#include <cmath>
#include <limits>
#include "pdollarrecognizer.h"
#include <QDebug>
#include <QVector>
#include <QPointF>

PDollarRecognizer::PDollarRecognizer(QObject *parent)
	:QObject(parent)
{
	clean();
	currentTemplates.clear();
}

void PDollarRecognizer::clean()
{
	// Empty toBeRecognize
	toBeRecognize.n = 0;
	toBeRecognize.gestureClass = "";
	toBeRecognize.points.clear();
}

void PDollarRecognizer::loadPDRTemplates(QString tmpDir)
{
	currentTemplates.clear();

	// Load all .dat files in this folder
	QString path = tmpDir;
	QDir dir(path);
	QStringList filters;
	filters << "*.dat";

	pdrTemplate tmp;
	pdrPoint pt;

	foreach(QString fileName, dir.entryList(filters,QDir::Files))
	{
		QFile file(dir.absoluteFilePath(fileName));
		if(!file.open(QIODevice::ReadOnly))
		{
			qDebug("Failed to open file");
		}
		else
		{
			// File Opened
			QDataStream in(&file);
			QString str;
			qint32 ii;
			double dd;

			// Clean up tmp
			tmp.gestureClass = "";
			tmp.n = 0;
			tmp.points.clear();

			in >> str;
			tmp.gestureClass = str;

			in >> ii;
			tmp.n = ii;

			in >> ii;
			if( ii != NUMBER_OF_POINTS_IN_TEMPLATE )
			{
				qDebug("This Template is illegal");
			}

			for(int i=0;i<NUMBER_OF_POINTS_IN_TEMPLATE;i++)
			{
				in >> dd;
				pt.x = dd;

				in >> dd;
				pt.y = dd;

				in >> ii;
				pt.strokeID = ii;

				tmp.points.push_back(pt);
			}

			currentTemplates.push_back(tmp);
		}
	}
}

void PDollarRecognizer::addPDRTemplate(QString gestureClass)
{
	toBeRecognize.gestureClass = gestureClass;
	currentTemplates.push_back(toBeRecognize);
}

void PDollarRecognizer::savePDRTemplate(QString tmpDir, QString gestureClass)
{
	Normalize();
	if( toBeRecognize.points.size() != NUMBER_OF_POINTS_IN_TEMPLATE )
	{
		qDebug()<<"Bad Input, Try Again";
	}

	QFile file(tmpDir);
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);   // we will serialize the data into the file

	/*
	* The following will be stored in order
	* QString gestureClass
	* qint32 number of strokes
	* qint32 number of total points
	* each points in order
	* double x
	* double y
	* qint32 strokeID
	*/

	out << QString(gestureClass);

	out << (qint32)toBeRecognize.n;

	out << (qint32)toBeRecognize.points.size();

	for(int i=0;i<toBeRecognize.points.size();i++)
	{
		out << (double)toBeRecognize.points[i].x;
		out << (double)toBeRecognize.points[i].y;
		out << (qint32)toBeRecognize.points[i].strokeID;
	}

	qDebug() << "Template Type " << gestureClass <<" added";

	// Add to current program
	addPDRTemplate(gestureClass);
}

void PDollarRecognizer::addStroke(QVector<QPointF> points)
{
	int numPoints = points.size();
	int strokeNumber = toBeRecognize.n + 1;
	toBeRecognize.n = strokeNumber;

	float x, y;
	pdrPoint tmp;
	for (QVector<QPointF>::const_iterator iter = points.constBegin();
		 iter != points.constEnd(); ++iter)
	{
		QPointF tmp_point = (*iter);
		x = tmp_point.x();
		y = tmp_point.y();

		tmp.strokeID = strokeNumber;
		tmp.x = x;
		tmp.y = y;

		toBeRecognize.points.push_back(tmp);
	}
}

QString PDollarRecognizer::gbRecognize()
{
	// The matching part

	Normalize();

	if(toBeRecognize.points.size()!=NUMBER_OF_POINTS_IN_TEMPLATE)
	{
		qDebug()<<"Bad Input, Try Again";
		return "";
	}

	double score = std::numeric_limits<double>::max();
	double d;
	QString result = "";

	for(int i=0;i<currentTemplates.size();i++)
	{
		d = GreedyCloudMatch(toBeRecognize, currentTemplates[i]);

		if( score > d )
		{
			score = d;
			result = currentTemplates[i].gestureClass;
		}
	}

	score = (std::max)((2.0-score)/2.0,0.0);

	// Some Display for debug
	qDebug() << "Gesture class is " << result << "with a score of" << score;
	emit onGestureDetect(result, score);
	return result;
}

void PDollarRecognizer::Normalize()
{
	// Will follow 100% the naming convention
	// in the Pseudocode

	/*
	 * 1. Resample
	 */
	double pathLength = 0.0;
	for(int i=1;i<toBeRecognize.points.size();i++)
	{
	   if(toBeRecognize.points[i].strokeID ==
			   toBeRecognize.points[i-1].strokeID )
	   {
		   pathLength = pathLength +
				   EuclideanDistance(toBeRecognize.points[i]
									 ,toBeRecognize.points[i-1]);
	   }
	}
	double I = pathLength / (NUMBER_OF_POINTS_IN_TEMPLATE - 1.0);

	double D = 0.0;
	double d;
	std::vector<pdrPoint> newPoints;
	std::vector<pdrPoint> copy = toBeRecognize.points;
	newPoints.push_back(copy[0]);

	pdrPoint tmp;
	for(int i=1;i<copy.size();i++)
	{
		if(copy[i].strokeID == copy[i-1].strokeID )
		{
			d = EuclideanDistance(copy[i],copy[i-1]);
			if(D+d >= I-0.001)
			{
				tmp.x = copy[i-1].x + ((I-D)/d) * (copy[i].x - copy[i-1].x);
				tmp.y = copy[i-1].y + ((I-D)/d) * (copy[i].y - copy[i-1].y);
				tmp.strokeID = copy[i].strokeID;
				newPoints.push_back(tmp);
				copy[i-1] = tmp;  // tmp will be next pi
				i = i-1;                          // tmp will be next pi
				D = 0.0;
			}
			else
			{
				D = D + d;
			}

		}
	}

	toBeRecognize.points = newPoints;

	/*
	 * 2. Scale
	 */
	double xmin = std::numeric_limits<double>::max();
	double xmax = 0.0;
	double ymin = std::numeric_limits<double>::max();
	double ymax = 0.0;


	for(int i=0;i<toBeRecognize.points.size();i++)
	{
		xmin = (std::min)(xmin,toBeRecognize.points[i].x);
		ymin = (std::min)(ymin,toBeRecognize.points[i].y);
		xmax = (std::max)(xmax,toBeRecognize.points[i].x);
		ymax = (std::max)(ymax,toBeRecognize.points[i].y);
	}

	double scale = std::max(xmax-xmin,ymax-ymin);

	for(int i=0;i<toBeRecognize.points.size();i++)
	{
		toBeRecognize.points[i].x = (toBeRecognize.points[i].x - xmin)/scale;
		toBeRecognize.points[i].y = (toBeRecognize.points[i].y - ymin)/scale;
	}

	/*
	 * 3. Translate to Origin
	 */
	// centroid
	double cx = 0.0;
	double cy = 0.0;

	for(int i=0;i<toBeRecognize.points.size();i++)
	{
		cx = cx + toBeRecognize.points[i].x;
		cy = cy + toBeRecognize.points[i].y;
	}
	cx = cx / (double)NUMBER_OF_POINTS_IN_TEMPLATE;
	cy = cy / (double)NUMBER_OF_POINTS_IN_TEMPLATE;

	for(int i=0;i<toBeRecognize.points.size();i++)
	{
		toBeRecognize.points[i].x = toBeRecognize.points[i].x - cx;
		toBeRecognize.points[i].y = toBeRecognize.points[i].y - cy;
	}
}


double PDollarRecognizer::GreedyCloudMatch(pdrTemplate pnt,pdrTemplate tmpl)
{
	double eps = 0.5;
	int step = std::floor( pow(NUMBER_OF_POINTS_IN_TEMPLATE, 1.0-eps) );
	double minDist = std::numeric_limits<double>::max();

	double d1,d2,md;
	for(int i=0;i<= step;i++)
	{
		d1 = CloudDistance(pnt,tmpl,i);
		d2 = CloudDistance(tmpl,pnt,i);
		md = (std::min)(d1,d2);
		minDist = (std::min)(minDist,md);
	}

	return minDist;
}

double PDollarRecognizer::CloudDistance(pdrTemplate pnt,pdrTemplate tmpl,int start)
{
	std::vector<bool> matched = std::vector<bool>(NUMBER_OF_POINTS_IN_TEMPLATE);
	double sum = 0.0;
	int i = start;  // start matching with points

	double minDist;
	double d;
	double weight;
	int index;
	int n = NUMBER_OF_POINTS_IN_TEMPLATE;
	do
	{
		minDist = std::numeric_limits<double>::max();
		for(int j=0;j<NUMBER_OF_POINTS_IN_TEMPLATE;j++)
		{
			if(matched[j] == false)
			{
				d = EuclideanDistance(pnt.points[i],tmpl.points[j]);
				if(d<minDist)
				{
					minDist = d;
					index = j;
				}
			}
		}
		matched[index] = true;
		weight = 1.0 - ( (i-start+n) % n ) / n;
		sum = sum + weight*minDist;
		i = (i+1) % n;
	}
	while(i != start);

	return sum;
}

double PDollarRecognizer::EuclideanDistance(pdrPoint p1, pdrPoint p2)
{
	double dist = 0.0;
	dist = (p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y);
	dist = sqrt(dist);

	return dist;
}
