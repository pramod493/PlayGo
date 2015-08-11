#ifndef CDIBOXDEBUGDRAW
#define CDIBOXDEBUGDRAW
#include <QtAlgorithms>
#include "box2dworld.h"
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>
#include <QGraphicsPolygonItem>
#include <QMessageBox>
#include <QDebug>
using namespace CDI;
class BoxDebugScene : public b2Draw
{
public:
	QGraphicsView* view;
	QGraphicsScene* scene;
	QList<QGraphicsItem*> itemsToDelete;

	float scale;
public:
	BoxDebugScene(float displayScale)
		:b2Draw()
	{
		scene = new QGraphicsScene(NULL);
		scale = displayScale;
	}

	void clear()
	{
		qDeleteAll(itemsToDelete);
		itemsToDelete.clear();
		scene->update();
	}

	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
	{
		QPolygonF polygon;
		for (int i=0; i < vertexCount; i++)
		{
			polygon.push_back(QPointF(vertices[i].x *scale,vertices[i].y *scale));
		}
		polygon.push_back(QPointF(vertices[0].x *scale,vertices[0].y *scale));
		QGraphicsPolygonItem* p = scene->addPolygon(polygon);
		QPen pen = QPen();
		pen.setColor(Qt::blue);
		pen.setWidthF(0.1);
		pen.setStyle(Qt::DotLine);
		p->setPen(QPen(Qt::blue));
		p->setBrush(QBrush(Qt::NoBrush));
		p->setFlag(QGraphicsItem::ItemIsMovable, true);
		scene->update(p->boundingRect());
		itemsToDelete.push_back(p);
	}
	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
	{
		QPolygonF polygon;
		for (int i=0; i < vertexCount; i++)
		{
			polygon.push_back(QPointF(vertices[i].x *scale,vertices[i].y *scale));
		}
		polygon.push_back(QPointF(vertices[0].x *scale,vertices[0].y *scale));
		QGraphicsPolygonItem* p = scene->addPolygon(polygon);
		QPen pen = QPen();
		pen.setColor(Qt::blue);
		pen.setWidthF(0.1);
		pen.setStyle(Qt::DotLine);
		p->setPen(QPen(Qt::blue));
		p->setBrush(QBrush(QColor(255,255,50,100), Qt::SolidPattern));

//		p->setFlag(QGraphicsItem::ItemIsMovable, true);
		scene->update(p->boundingRect());
		itemsToDelete.push_back(p);
	}

	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
	{
		QGraphicsEllipseItem* ellipse = scene->addEllipse(QRectF(center.x-radius, center.y-radius, 2*radius, 2* radius));
		ellipse->setPen(QPen(Qt::red));
		scene->update(ellipse->boundingRect());
		itemsToDelete.push_back(ellipse);
	}

	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
	{
		DrawCircle(center, radius, color);
	}
	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
	{
		QGraphicsLineItem* line = scene->addLine(
					QLineF(QPointF(p1.x,p1.y)*scale, QPointF(p2.x,p2.y)*scale));
		line->setPen(QPen(Qt::green));
		scene->update(line->boundingRect());
		itemsToDelete.push_back(line);
	}
	void DrawTransform(const b2Transform& xf)
	{
		float mul = 1.0f;	// see if it impacts the positioning of transform
		b2Vec2 p1 = xf.p;
		b2Vec2 p2 = p1 + xf.q.GetXAxis();
		QPen pen = QPen(Qt::green);
		pen.setWidth(3);

		QLineF xAxis = QLineF(QPointF(p1.x,p1.y)*scale*mul, QPointF(p2.x,p2.y)*scale*mul);
		QGraphicsLineItem* xline = scene->addLine(xAxis); xline->setPen(pen);

		p2 = p1 + xf.q.GetYAxis();
		QLineF yAxis = QLineF(QPointF(p1.x,p1.y)*scale * mul, QPointF(p2.x,p2.y)*scale *mul);
		QGraphicsLineItem* yline = scene->addLine(yAxis); yline->setPen(pen);
		itemsToDelete.push_back(xline);
		itemsToDelete.push_back(yline);
	}
};

#endif // CDIBOXDEBUGDRAW

