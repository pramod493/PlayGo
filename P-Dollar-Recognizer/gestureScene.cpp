#include "gestureScene.h"

GestureScene::GestureScene()
	:QGraphicsScene(NULL)
{
	strokes = QVector<GraphicsPathItem*>();
	currentStroke = NULL;
}

void GestureScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	currentStroke = new GraphicsPathItem();
	addItem(currentStroke);
	strokes.push_back(currentStroke);
	currentStroke->points.push_back(mouseEvent->scenePos());
	update();
}

void GestureScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	currentStroke->points.push_back(mouseEvent->scenePos());
	update();
}

void GestureScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
	currentStroke->points.push_back(mouseEvent->scenePos());
	update();
}

GraphicsPathItem::GraphicsPathItem() :QGraphicsPathItem()
{
	points = QVector<QPointF>();
}

void GraphicsPathItem::paint(QPainter *painter,
							 const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (points.size() < 2) return;
	QPen pen = QPen(Qt::black);
	pen.setWidth(3);

	painter->setPen(pen);
	for (int i=1; i< points.size(); i++)
	{
		painter->drawLine(points[i-1], points[i]);
	}
}
