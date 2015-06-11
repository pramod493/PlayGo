#pragma once
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QVector>
#include <QPointF>
#include <QPainter>

class GraphicsPathItem;
class GestureScene : public QGraphicsScene
{
	Q_OBJECT
public:
	QVector<GraphicsPathItem*> strokes;

	GestureScene();

	void addStroke(GraphicsPathItem* item);

protected:
	GraphicsPathItem* currentStroke;

	void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);

	void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) ;

	void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) ;
};

class GraphicsPathItem : public QGraphicsPathItem
{
public:
	QVector<QPointF> points;

	GraphicsPathItem();

	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
};
