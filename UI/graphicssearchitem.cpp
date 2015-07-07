#include "graphicssearchitem.h"
#include "QsLog.h"
#include "searchview.h"
#include <QFile>
namespace CDI
{
	GraphicsSearchItem::GraphicsSearchItem
	(SearchResult* searchresult, int dim, QGraphicsItem* parent)
		: QGraphicsPixmapItem(parent)
	{
		if (dim <= 0) dim = 150;
		_id = uniqueHash();

		_parentResult = searchresult;

		QFile f(_parentResult->resultFilePath);
		if (f.exists())
		{
			QPixmap pixmap = QPixmap();
			pixmap.load(_parentResult->resultFilePath);
			pixmap = pixmap.scaled(QSize(dim, dim), Qt::KeepAspectRatio);
			setPixmap(pixmap);
		}
		setShapeMode(QGraphicsPixmapItem::BoundingRectShape);

		setAcceptTouchEvents(true);
		setFlag(QGraphicsItem::ItemIgnoresTransformations,true);

		_searchView = NULL;
	}

	GraphicsSearchItem::~GraphicsSearchItem()
	{
		// Do not delete _parentResult as it is associated with Component.
	}

	SearchResult* GraphicsSearchItem::getSearchResult()
	{
		return _parentResult;
	}

	void GraphicsSearchItem::setView(SearchView *view)
	{
		_searchView = view;
	}

	QDataStream& GraphicsSearchItem::serialize(QDataStream& stream) const
	{
		stream << _id;
		return stream;
	}
	QDataStream& GraphicsSearchItem::deserialize(QDataStream& stream)
	{
		stream >> _id;
		return stream;
	}

	/*void GraphicsSearchItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
	{
		QLOG_INFO() << "Search result clicked" << _parentResult->resultFilePath;
		if (_searchView != NULL)
		{
			_searchView->onSearchResultSelect(_parentResult);
		}
		QGraphicsPixmapItem::mousePressEvent(event);
	}*/

	void GraphicsSearchItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
	{
		if (_searchView != NULL)
		{
			_searchView->onSearchResultSelect(_parentResult);
		}
	}

	bool GraphicsSearchItem::sceneEvent(QEvent *sEvent)
	{
		switch (sEvent->type())
		{
		case QEvent::TouchBegin :
		case QEvent::TouchUpdate :
		case QEvent::TouchEnd :
		case QEvent::TouchCancel :
		{
			sEvent->accept();
			QTouchEvent* event = static_cast<QTouchEvent*>(sEvent);
			// Do not process when the touch event arrives
			if (event->type()==QEvent::TouchBegin) return true;

			if (event->touchPoints().count() == 1)
			{
				// Single finger. Drag
				const QTouchEvent::TouchPoint &tp1 = event->touchPoints().first();
				QPointF pan = tp1.scenePos() - tp1.lastScenePos();
				moveBy(pan.x(), pan.y());

			} /*else if (event->touchPoints().count() == 2 && event->type()==QEvent::TouchUpdate)
			{
				// Two finger
				const QTouchEvent::TouchPoint &tp1 = event->touchPoints()[0];
				const QTouchEvent::TouchPoint &tp2 = event->touchPoints()[1];
				QPointF tp1_itemPos = mapFromScene(tp1.scenePos());
				QPointF tp2_itemPos = mapFromScene(tp2.scenePos());
				QPointF tp1_lastItemPos = mapFromScene(tp1.lastScenePos());
				QPointF tp2_lastItemPos = mapFromScene(tp2.lastScenePos());
				QPointF pan = QPointF(); float rot = 0; float scaleMul = 1;
				{
					pan = ( tp1.scenePos() + tp2.scenePos()
							- tp1.lastScenePos() - tp2.lastScenePos()
							) / 2.0f;
					moveBy(pan.x(), pan.y());
				}
				QPointF tmpOrigin =(tp1.scenePos() + tp1.lastScenePos())/2.0f;
				setTransformOriginPoint(mapFromScene(tmpOrigin));
				{
					float prevmag = euclideanDistance(&tp2_lastItemPos, &tp1_lastItemPos);
					float currentmag = euclideanDistance(&tp2_itemPos, &tp1_itemPos);
					scaleMul = scale() * currentmag / prevmag;
					setScale(scaleMul);
				}
				{
					// Offset previous event points
					tp1_lastItemPos = tp1_lastItemPos + pan;
					tp2_lastItemPos = tp2_lastItemPos + pan;
					QVector2D v1 = QVector2D(tp1_itemPos - tp2_itemPos);
					QVector2D v2 = QVector2D(tp1_lastItemPos - tp2_lastItemPos);
					rot = atan2(v1.y(), v1.x()) - atan2(v2.y(), v2.x());
					rot = rotation() + (rot * 180.0f) / _PI_;
					setRotation(rot);
				}

				if (event->type() == QEvent::TouchEnd || event->type() == QEvent::TouchCancel)
				{
					setTransformOriginPoint(0,0);
				}
			}*/
			return true;
		}
		}
		return QGraphicsPixmapItem::sceneEvent(sEvent);
	}
}
