#include "converttopolygons.h"
#include <QFileDialog>
#include "commonfunctions.h"
#include <QDebug>
#include <QGraphicsPolygonItem>
#include <QGraphicsPixmapItem>
#include <QPixmap>
#include "QsLog.h"

using namespace CDI;

void Polygonize::openFile()
{
	scene->clear();

	QString fileName = QFileDialog::getOpenFileName(_window, tr("Open Image"),
													"C:/Database/images/",
													tr("Images (*.png *.jpg)"));

	QPixmap pixmap = QPixmap();
	pixmap.load(fileName);
	QGraphicsPixmapItem* imageItem = new QGraphicsPixmapItem(pixmap);
	scene->addItem(imageItem);

	vector<p2t::Triangle*> trias = generatePolygonFromImage(fileName, 5, 5);
	QLOG_INFO() << trias.size() << "triangles created";

	QPen pen = QPen();
	pen.setColor(Qt::red);
	pen.setWidth(3);
	QBrush brush = QBrush(Qt::DiagCrossPattern);
	brush.setColor(Qt::cyan);

	for (int i=0; i< trias.size(); i++)
	{
		QPolygonF polygon = QPolygonF();
		p2t::Point *pt = 0;
		p2t::Triangle* tria = trias[i];

		pt = tria->GetPoint(0);	polygon.push_back(Point2D(pt->x,pt->y));
		pt = tria->GetPoint(1);	polygon.push_back(Point2D(pt->x,pt->y));
		pt = tria->GetPoint(2);	polygon.push_back(Point2D(pt->x,pt->y));
//		pt = tria->GetPoint(0);	polygon.push_back(Point2D(pt->x,pt->y));

		QGraphicsPolygonItem* item = new QGraphicsPolygonItem(polygon);
		item->setPen(pen);
		item->setBrush(brush);
		scene->addItem(item);
	}

	scene->update();

}


