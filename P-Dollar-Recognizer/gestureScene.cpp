#include "gestureScene.h"
#include <qdebug.h>

GestureScene::GestureScene()
	:QGraphicsScene(NULL)
{
	strokes = QVector<GraphicsPathItem*>();
	currentStroke = NULL;

	pdRecognizer = new PDollarRecognizer();

	// loadTemplate
#ifdef Q_OS_LINUX
	QString dataDir = "/home/pramod/Junks/database/gestures/";
#else
	QString dataDir = "C:/Database/gestures/";
#endif //Q_OS_LINUX
	pdRecognizer->loadPDRTemplates(dataDir);
}

void GestureScene::addStroke(GraphicsPathItem *item)
{
	addItem(item);
	strokes.push_back(item);
	update();
}

void GestureScene::clearStrokes()
{
	strokes.clear();
	QGraphicsScene::clear();
	update();

	// Also please clear up data in Recognizer
	pdRecognizer->clean();

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

	// Save CurrentStroke Data to PDRRecognizer
	SaveCurrentStrokeToPDR();

}

void GestureScene::SaveCurrentStrokeToPDR()
{
	int numPoints = currentStroke->points.size();

	// Update Total number of stroke
	int strokeNumber = pdRecognizer->toBeRecognize.n + 1;
	pdRecognizer->toBeRecognize.n = strokeNumber;

	float x,y;    // coordinate of points
	pdrPoint tmp;   // Temporary Holder
	for(int i=0;i<numPoints;i++)
	{
		x = currentStroke->points[i].x();
		y = currentStroke->points[i].y();

		tmp.strokeID = strokeNumber;
		tmp.x = x;
		tmp.y = y;

		pdRecognizer->toBeRecognize.points.push_back(tmp);
	}
}

GraphicsPathItem::GraphicsPathItem() :QGraphicsPathItem()
{
	points = QVector<QPointF>();
}

void GraphicsPathItem::paint(QPainter *painter,
							 const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option)
	Q_UNUSED(widget)
	if (points.size() < 2) return;
	QPen pen = QPen(Qt::black);
	pen.setWidth(3);

	painter->setPen(pen);
	for (int i=1; i< points.size(); i++)
	{
		painter->drawLine(points[i-1], points[i]);
	}
}
