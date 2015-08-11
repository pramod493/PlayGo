#include "searchview.h"
#include <QtAlgorithms>
#include "QsLog.h"
#include "searchresult.h"
#include <QMessageBox>

namespace CDI
{
	SearchView::SearchView(QWidget *parent)
		: QGraphicsView(parent)
	{
		QGraphicsScene* tmp = new QGraphicsScene(this);
		setScene(tmp);

        //setAttribute(Qt::WA_AcceptTouchEvents, true);
		viewport()->setAttribute(Qt::WA_AcceptTouchEvents, true);

		setRenderHint(QPainter::Antialiasing);
		setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
		setTransformationAnchor(QGraphicsView::NoAnchor);

		imgDim = 100;
		margin = 10;
	}

	SearchView::~SearchView()
	{
		if (scene() != NULL)
			delete scene();
	}

	void SearchView::clear()
	{
		_selectedStrokes.clear();

		if (_searchDispItems.size())
			qDeleteAll(_searchDispItems);
		_searchDispItems.clear();

	}

	void SearchView::mousePressEvent(QMouseEvent *event)
	{
		//QMessageBox::about(this, QString("Search view"), QString("Search view"));
        QGraphicsView::mousePressEvent(event);
	}

	bool SearchView::viewportEvent(QEvent *event)
	{
        return QGraphicsView::viewportEvent(event);

		switch(event->type())
		{
		case QEvent::TouchBegin :
		case QEvent::TouchUpdate :
		case QEvent::TouchEnd :
		case QEvent::TouchCancel :
		{
			QTouchEvent* touchEvent = static_cast<QTouchEvent*>(event);
			if (touchEvent->touchPoints().count() == 2)
			{
				event->accept();
				//			QList<QTouchEvent::TouchPoint> touchpoints = event->touchPoints();
				const QTouchEvent::TouchPoint &tp1 = touchEvent->touchPoints()[0];
				const QTouchEvent::TouchPoint &tp2 = touchEvent->touchPoints()[1];

				// 1. Scaling by increase in radius
				QPointF previousCenter = transform().map(0.5f* (tp1.lastPos() + tp2.lastPos()));
				QPointF currentCenter = transform().map(0.5f* (tp1.pos() + tp2.pos()));

				QPointF difference = currentCenter - previousCenter;
				translate(difference.x(),difference.y());

				QPointF p1 = tp1.lastScenePos() - tp2.lastScenePos();
				QPointF p2 = tp1.scenePos() - tp2.scenePos();
				float scaleFactor = magnitude(&p2)/magnitude(&p1);
				if (qFuzzyIsNull(scaleFactor) == false && qFuzzyCompare(scaleFactor, 0))
					scale(scaleFactor, scaleFactor);
				// 2. Translate by motion of center
				// 3. Rotate by mean of rotation of each point by center
				return true;
			}

		}
		}
		return QGraphicsView::viewportEvent(event);
	}

	void SearchView::LoadSearchData
	(QList<Stroke *> &selectedStrokes, QList<SearchResult *> &searchResults)
	{
		clear();
		_selectedStrokes = selectedStrokes;

		for (int i=0; i< searchResults.size(); i++)
		{
			SearchResult* searchresult = searchResults[i];
			GraphicsSearchItem* searchItem = new GraphicsSearchItem(searchresult, imgDim);
			scene()->addItem(searchItem);
			// Set current view as parent view in the object
			searchItem->setView(this);
			_searchDispItems.push_back(searchItem);
		}
		//graphicsView->mapToScene(graphicsView->viewport()->geometry()).boundingRect()
		QRectF sceneRect = mapToScene(viewport()->geometry()).boundingRect();
		for (int i=0; i< _searchDispItems.size(); i++)
		{
			GraphicsSearchItem* searchItem = _searchDispItems[i];
			searchItem->setPos((i%5) * (imgDim+margin),
						(i/5) * (imgDim+margin));
            //searchItem->setTransform(searchItem->transform().rotate( (i%5)*10.0f, Qt::YAxis ));
            //searchItem->setTransform(searchItem->transform().rotate( (i/5)*10.0f, Qt::XAxis ));
		}
	}

	void SearchView::onSearchResultSelect(SearchResult* result)
	{

		emit signalOnSearchResultSelect(result);
	}
}
