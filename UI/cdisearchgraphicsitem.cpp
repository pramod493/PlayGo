#include "cdisearchgraphicsitem.h"
#include <QGraphicsScene>
#include "QsLog.h"
#include "sketchscene.h"
#include <QGraphicsSceneMouseEvent>

namespace CDI {
	cdSearchGraphicsItem::cdSearchGraphicsItem(
			SearchResult* searchResult, int dim, QGraphicsItem* parent)
		: QObject(), QGraphicsRectItem(parent)
	{
		if (dim < 0) dim = 150;

		setPen(QPen(Qt::green));
		setBrush(QBrush(QColor(20,20,20,200)/*, Qt::BDiagPattern*/));
		setRect(0,0,dim, dim);

		setAcceptDrops(true);
		setZValue(Z_UIVIEW);
		setAcceptTouchEvents(true);
		setFlag(QGraphicsItem::ItemIgnoresTransformations);

		result = SearchResult(*searchResult);
		QString sourceFilePath = result.resultFilePath;

		QPixmap pixmap = QPixmap(sourceFilePath);
		QPixmap cropped = pixmap.scaled(dim, dim, Qt::KeepAspectRatio);
		QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(cropped, this);
		pixmapItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);

		touchOn = false;
		waitInMiliseconds = 500;
		t = QTime();
	}

	bool cdSearchGraphicsItem::sceneEvent(QEvent* event)
	{
		switch (event->type())
		{
		case QEvent::TouchBegin :
		case QEvent::GraphicsSceneMousePress :
		{
			touchOn = true;		// touch began
			t.restart();		// start timer
			event->accept();
			return true;
		}
		case QEvent::TouchEnd :
		{
			if (touchOn && (t.elapsed() < waitInMiliseconds))
			{
				touchOn = false;
				QTouchEvent *touchEvent = static_cast<QTouchEvent*>(event);
				QList<QTouchEvent::TouchPoint> touchpoints = touchEvent->touchPoints();
				for(auto tp : touchpoints)
				{
					QPointF scenePos = tp.scenePos();
					if (contains(mapFromScene(scenePos)))
					{
						emit signalSearchItemSelected(this);
						return true;
					}
				}
			}
			break;
		}
		case QEvent::GraphicsSceneMouseRelease :
		{
			if (touchOn && (t.elapsed() < waitInMiliseconds))
			{
				touchOn = false;
				QGraphicsSceneMouseEvent* mouseEvent =
						static_cast<QGraphicsSceneMouseEvent*>(event);
				QPointF scenePos = mouseEvent->scenePos();
				if (contains(mapFromScene(scenePos)))
				{
					emit signalSearchItemSelected(this);
					return true;
				}
			}
			break;
		}
		default:
			break;
		}
		return false;
	}

	SearchResult cdSearchGraphicsItem::getResult() const
	{
		return result;
	}

}
