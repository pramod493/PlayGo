#include <QCoreApplication>
#include <QtGui/QtGui>
#include <QPixmap>
#include <QBitmap>
#include "commonfunctions.h"
#include "point2dpt.h"
#include "stroke.h"
#include "component.h"
#include <QDebug>

#include <QFile>

using namespace std;
using namespace CDI;

void testRandomNumberGenerator()
{
	qDebug() << uniqueHash().toString();
	qDebug() << uniqueHash().toString();
	qDebug() << uniqueHash().toString();
	qDebug() << uniqueHash().toString();
	qDebug() << uniqueHash().toString();
	qDebug() << uniqueHash().toString();
	qDebug() << uniqueHash().toString();
}

bool testPointOperations()
{
	qDebug() << "Creating new point";

	Point2DPT point = Point2DPT(3.0f,9.0f,0.75f,200);
	point = point*2.5f;
	qDebug() << "Point created " << point;

	// Testing file IO
	QFile file("file.dat");
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);
	out << point;
	file.close();

	qDebug() << "Data written to file." << "Now attempting to read data";

	file.open(QIODevice::ReadOnly);
	QDataStream in(&file);
	Point2DPT p2;

	in >> p2;
	file.close();

	qDebug() <<"Printing read data";
	qDebug() << p2;

	return true;
}

bool testStrokeOperations()
{
	Stroke s = Stroke();
	s.setColor(Qt::black);
	s.setThickness(0.5f);
	s.push_back(Point2DPT(10,2,30,4));
	s.push_back(Point2DPT(50,60,7,8));
	s.push_back(Point2DPT(10,2,30,4));
	s.push_back(Point2DPT(50,60,7,8));
	s.push_back(Point2DPT(10,2,30,4));
	s.push_back(Point2DPT(50,60,7,8));

	s.setTransform(s.transform().rotate(3.5f));

	// Testing file IO
	QFile file("file.dat");
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);
	out << s;
	file.close();

	qDebug() << "Stroke Data" << s;

	qDebug() << "Data written to file." << "Now attempting to read data";

	file.open(QIODevice::ReadOnly);
	QDataStream in(&file);
	Stroke s2 = Stroke();

	in >> s2;
	file.close();

	qDebug() << "Stroke Data" << s2;
	return true;
}

bool testStrokePointerOperations()
{
	Stroke* s = new Stroke(Qt::blue, 3.5f);

	s->push_back(Point2DPT(10,2,30,4));
	s->push_back(Point2DPT(50,60,7,8));

	s->push_back(Point2DPT(10,2,30,4));
	s->push_back(Point2DPT(50,60,7,8));

	s->push_back(Point2DPT(10,2,30,4));
	s->push_back(Point2DPT(50,60,7,8));

	s->setTransform(s->transform().rotate(3.5f));

	// Testing file IO
	QFile file("file.dat");
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);
	out << *s;
	file.close();

	qDebug() << "Stroke Data" << *s;

	qDebug() << "Data written to file." << "Now attempting to read data";

	file.open(QIODevice::ReadOnly);
	QDataStream in(&file);
	Stroke s2 = Stroke();

	in >> s2;
	file.close();

	qDebug() << "Stroke Data" << s2;
	return true;
}

bool testComponentSerializations()
{
	Component* component = new Component();
	for (int i=2; i<6; i++)
	{
		Stroke* s = new Stroke();
		for (int j=0; j< i; j++)
		{
			s->push_back(Point2DPT(qrand()/100.f, qrand()/100.f, qrand()/100.f,qrand()));
		}
		component->insert(s->id().toString(), static_cast<AbstractModelItem*>(s));
	}

	QFile file("file.dat");
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);
	out << *component;
	file.close();

	Component *newComponent = new Component();
	file.open(QIODevice::ReadOnly);
	QDataStream in(&file);
	in >> *newComponent;
	file.close();

	return false;
}

void testImageSerializeOperations()
{
	QFile file("file.dat");
	QString imagefile("E:/Camera.png");
	if (0){
		Image s = Image(imagefile);

		qDebug() << "Object created";

		s.setTransform(s.transform().rotate(3.5f));
		qDebug() << "Testing Image atributes" << s.pixmap().size();

		qDebug() << "Writing Image object to file";

		file.open(QIODevice::WriteOnly);
		QDataStream out(&file);
		out << s;
		file.close();
	}
	{
		QPixmap pixmap = QPixmap();
		pixmap.load(imagefile);
		Image s = Image(pixmap, "tomata.png");

		qDebug() << "Object created from iamge";

		s.setTransform(s.transform().rotate(3.5f));
		qDebug() << "Testing Image atributes" << s.pixmap().size();

		qDebug() << "Writing Image object to file";

		file.open(QIODevice::WriteOnly);
		QDataStream out(&file);
		out << s;
		file.close();
	}
	{
		qDebug() << "Attempting to read the image information";
		file.open(QIODevice::ReadOnly);
		QDataStream in(&file);
		Image s2 = Image();
		in >> s2;
		file.close();
		qDebug() << "Testing Image atributes" << s2.pixmap().size();

		QString image_new_name("tomato.png");
		qDebug() << "Attempting to save image with name " << image_new_name;
		s2.pixmap().save(image_new_name);
	}
}

void testPixmapSerialization()
{
	QFile file("file.dat");
	{
		QPixmap image = QPixmap("E:/Camera.png");
		qDebug() << "Testing Image atributes" << image.size();

		qDebug() << "Writing Image object to file";

		file.open(QIODevice::WriteOnly);
		QDataStream out(&file);
		QString test("random string - CDESIGN");
		out << test;
		out << image;
		file.close();
	}
	{
		QPixmap image = QPixmap();
		qDebug() << "Testing Image atributes" << image.size();

		qDebug() << "Loading image data from file";

		file.open(QIODevice::ReadOnly);
		QDataStream in(&file);
		QString test;
		in >> test;
		in >> image;
		file.close();
		qDebug() << "Check string read" << test;
		qDebug() << "Testing Image atributes" << image.size();
		QString newName("Caeabjjkea.png");
		image.save(newName);
		qDebug() << "SAVED AS " << newName;
	}
}


void testPixmapMask()
{
	QPixmap pixmap = QPixmap();
	pixmap.load("E:/Camera.png");

	QBitmap bitmap = pixmap.mask();
	bitmap.save("E:/bitmap_mask.png");
}

int main (int argc, char** argv)
{
	QApplication app(argc, argv);
	testPixmapMask();
	return app.exec();
}
