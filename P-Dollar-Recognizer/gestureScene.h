#pragma once
#include <QGraphicsScene>
#include <QGraphicsPathItem>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QVector>
#include <QPointF>
#include <QPainter>

#include <pdollarrecognizer.h>


class GraphicsPathItem;
class GestureScene : public QGraphicsScene
{
	Q_OBJECT
public:
	QVector<GraphicsPathItem*> strokes;

	GestureScene();



public:
    // Adrian's part
    PDollarRecognizer* pdRecognizer;

    // Some help function
    void addStroke(GraphicsPathItem* item);
    void clearStrokes();
    void SaveCurrentStrokeToPDR();

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
