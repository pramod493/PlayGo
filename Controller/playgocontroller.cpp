#include "PlayGoController.h"
#include <QObject>
#include <QDebug>
#include "QsLog.h"
#include "cdiwindow.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsWidget>
#include <QPixmap>
#include <QPixmap>
#include <QRectF>
#include "searchview.h"
#include "graphicssearchitem.h"
#include <QMessageBox>
#include "grabcut.h"
#include <QCamera>
#include <QCameraImageCapture>

namespace CDI
{

class Button : public QGraphicsWidget
{
    Q_OBJECT
public:
    Button(const QPixmap &pixmap, QString filePath, QGraphicsItem *parent = 0)
        : QGraphicsWidget(parent), _pix(pixmap)
    {
        _filePath = filePath;
        setAcceptHoverEvents(true);
        setCacheMode(DeviceCoordinateCache);
    }

    QRectF boundingRect() const Q_DECL_OVERRIDE
    {
        return QRectF(_pix.rect());
    }

    QPainterPath shape() const Q_DECL_OVERRIDE
    {
        QPainterPath path;
        path.addEllipse(boundingRect());
        return path;
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *) Q_DECL_OVERRIDE
    {
        bool down = option->state & QStyle::State_Sunken;
        QRectF r = boundingRect();
        QLinearGradient grad(r.topLeft(), r.bottomRight());
        grad.setColorAt(down ? 1 : 0, option->state & QStyle::State_MouseOver ? Qt::white : Qt::lightGray);
        grad.setColorAt(down ? 0 : 1, Qt::darkGray);
        painter->setPen(Qt::darkGray);
        painter->setBrush(grad);
        painter->drawEllipse(r);
        QLinearGradient grad2(r.topLeft(), r.bottomRight());
        grad.setColorAt(down ? 1 : 0, Qt::darkGray);
        grad.setColorAt(down ? 0 : 1, Qt::lightGray);
        painter->setPen(Qt::NoPen);
        painter->setBrush(grad);
        if (down)
            painter->translate(2, 2);
        painter->drawEllipse(r.adjusted(5, 5, -5, -5));
        painter->drawPixmap(-_pix.width()/2, -_pix.height()/2, _pix);
    }

signals:
    void pressed(QString filepath);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *) Q_DECL_OVERRIDE
    {
//        emit pressed();
//        update();
    }

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *) Q_DECL_OVERRIDE
    {
//        update(_filePath);
    }

private:
    QPixmap _pix;
    QString _filePath;
};

	PlayGoController::PlayGoController(SketchScene *scene, SketchView *view, CDIWindow *parent)
		:QObject(parent)
	{
        // Hide display of the model tree...
//		tree = new ModelViewTreeWidget(parent->playgo);
//		tree->show();
		tree = NULL;

		_toplevelWindow = parent;
		if (scene != NULL && view != NULL)
		{
			_scene = scene;
			_view = view;
			_page = _scene->page();

			view->setTransformationAnchor(QGraphicsView::NoAnchor);

			connect(_view, SIGNAL(viewTabletEvent(QTabletEvent*,QGraphicsView*)),
					this, SLOT(onTabletEventFromView(QTabletEvent*,QGraphicsView*)));

			connect(_scene, SIGNAL(signalMouseEvent(QGraphicsSceneMouseEvent*,int)),
					this, SLOT(onMouseEventFromScene(QGraphicsSceneMouseEvent*,int)));

			connect(_view, SIGNAL(viewTouchEvent(QTouchEvent*,QGraphicsView*)),
					this, SLOT(onTouchEventFromView(QTouchEvent*,QGraphicsView*)));

			connect(_view, SIGNAL(viewDrawforeground(QPainter*,const QRectF&)),
					this, SLOT(drawMenusOnView(QPainter*,const QRectF&)));

			connect(_view, SIGNAL(viewImageDrop(QString)),
					this, SLOT(loadImage(QString)));
		} else
		{
			QLOG_FATAL() << "Invalid scene and view reference.";
		}

        initController();

		searchView = _toplevelWindow->searchView;
		if (searchView != NULL)
		{
			connect(searchView, SIGNAL(signalOnSearchResultSelect(SearchResult*)),
					this, SLOT(onSearchItemSelect(SearchResult*)));
		}
	}

	void PlayGoController::initController()
	{
		// Set the deafult value of other settings
		_mouseModeEnabled = true;
        _activeMode = UI::Sketch;
		_device = QTabletEvent::NoDevice;

		_defaultPen = QPen(Qt::black);
		_defaultPen.setStyle(Qt::SolidLine);
		_defaultPen.setWidthF(3.5f);

		_fillPen = QPen(_defaultPen);

		_lassoPen = QPen(QColor(255,50,50,150));
		_lassoPen.setStyle(Qt::DashLine);
		_lassoPen.setWidthF(2.5f);

		_highlightPen = QPen(QColor(255,0,0,200));
		_highlightPen.setStyle(Qt::SolidLine);
		_highlightPen.setWidthF(5.0f);

		_defaultBrush = QBrush(Qt::NoBrush);

		_fillBrush = QBrush(QColor(0.75,0.75,0.75), Qt::SolidPattern);
		_lassoBrush = QBrush(Qt::NoBrush);
		_highlightBrush = QBrush(Qt::NoBrush);

		_isDrawingNow = false;
		_currentStroke = NULL;

		_lasso = new QGraphicsPolygonItem();
		_lasso->setPen(_lassoPen);
		_lasso->setBrush(_lassoBrush);
		_scene->addItem(_lasso);
		_isLassoDisplayed = false;

		_itemHighlighted = false;
	}

	void PlayGoController::brushPress(QPointF pos, float pressure, int time)
	{
		brushPress(Point2DPT(pos.x(),pos.y(),pressure, time));
	}

	void PlayGoController::brushPress(Point2DPT pos)
	{
		// Do not delete current stroke if it is not null
		if (_currentStroke != NULL)	_currentStroke = NULL;
		if (_page == NULL) return;
		if (_page->currentComponent() == NULL)
		{
			Component* currentComponent = _page->createComponent();
			QTransform t;
			t = t.translate(pos.x(), pos.y());
			currentComponent->setTransform(t);
			_page->setCurrentComponent(currentComponent);
		}
		_page->currentComponent()->setZValue(1);
		_currentStroke = new Stroke();
		_currentStroke->setPen(_defaultPen);
		_currentStroke->setBrush(_defaultBrush);

		_page->currentComponent()->addToComponent(_currentStroke);
		_currentStroke->push_point(pos);
		_isDrawingNow = true;
	}

	void PlayGoController::brushPress(float x, float y, float pressure, int time)
	{
		brushPress(Point2DPT(x,y,pressure,time));
	}

	void PlayGoController::brushMove(QPointF pos, float pressure, int time)
	{
		brushMove(Point2DPT(pos.x(),pos.y(),pressure, time));
	}

	void PlayGoController::brushMove(Point2DPT pos)
	{
		if (_page == NULL) return;
		if (!(_currentStroke == NULL) && _isDrawingNow)
			_currentStroke->push_point(pos);
		// TODO - Need not update scene because object shuld take care of it
		_scene->update(_currentStroke->mapRectToScene(_currentStroke->boundingRect()));
	}

	void PlayGoController::brushMove(float x, float y, float pressure, int time)
	{
		brushMove(Point2DPT(x, y, pressure, time));
	}

	void PlayGoController::brushRelease(QPointF pos, float pressure, int time)
	{
		brushRelease(Point2DPT(pos.x(),pos.y(),pressure, time));
	}

	void PlayGoController::brushRelease(Point2DPT pos)
	{
		if (_page == NULL) return;
		if (_currentStroke == NULL || !_isDrawingNow) return;
		_isDrawingNow = false;
		_currentStroke->push_point(pos);
		_currentStroke->applySmoothing(2);

		emit signalStrokeComplete(_currentStroke);
		// TODO Apply smoothing before the object is updated maybe?
		_currentStroke = NULL;

		if (_page->currentComponent())_page->currentComponent()->recalculateBoundingRect();
	}

	void PlayGoController::brushRelease(float x, float y, float pressure, int time)
	{
		brushRelease(Point2DPT(x,y,pressure,time));
	}

	void PlayGoController::eraserPress(Point2DPT pos)
	{
		eraserMove(pos);
	}

	void PlayGoController::eraserMove(Point2DPT pos)
	{
		QList<Stroke*> selectedStrokes = _scene->getSelectedStrokes(Point2D(pos.x(),pos.y()), _defaultPen.widthF());
		for (int i=0; i< selectedStrokes.size(); i++)
			delete selectedStrokes[i];
	}

	void PlayGoController::eraserRelease(Point2DPT pos)
	{
		eraserMove(pos);
	}

	void PlayGoController::lassoPress(Point2DPT pos)
	{
		_isLassoDisplayed = true;
		_lassoPolygon = QPolygonF();
		_lassoPolygon.push_back(pos);
		_lasso->setPolygon(_lassoPolygon);
		_lasso->update(_lasso->boundingRect());
	}

	void PlayGoController::lassoMove(Point2DPT pos)
	{
		_lassoPolygon.push_back(pos);
		_lasso->setPolygon(_lassoPolygon);
		_lasso->update(_lasso->boundingRect());
	}

	void PlayGoController::lassoRelease(Point2DPT pos)
	{
		_lassoPolygon.push_back(pos);
		//_lasso->setPolygon(_lassoPolygon);
		_lasso->update(_lasso->boundingRect());

		QList<Stroke*> selectedStrokes = _scene->getSelectedStrokes(_lassoPolygon, 1.0f);
        if (selectedStrokes.size()) _itemHighlighted = true;
        for (int i=0; i < selectedStrokes.size(); i++)
        {
            selectedStrokes[i]->highlight(true);
        }
//		QRectF boundingBox = _lassoPolygon.boundingRect();
//		QPointF center = boundingBox.center();
//		_scene->addEllipse(center.x(), center.y(), 100.0f, 100.0f,
//						   _lassoPen,_fillBrush);
		_lasso->setPolygon(QPolygonF());
		_isLassoDisplayed = false;
	}

    void PlayGoController::onModeChange(UI::MODE oldmode, UI::MODE newmode)
    {
        if (oldmode == UI::Select)
        {
			if (newmode == UI::Erase) // delete highlight
			{
				QList<Stroke*> highlihghtedItems = _scene->getHighlightedStrokes();
				for (int i=0; i < highlihghtedItems.size(); i++)
					delete highlihghtedItems[i];
			}
            if (_itemHighlighted)
                _scene->clearStrokeHighlight();
            _isLassoDisplayed = false;
            _lassoPolygon.clear();
            _lasso->setPolygon(_lassoPolygon);
        }

        if (oldmode == UI::Sketch)
        {
            _currentStroke = NULL;
            _isDrawingNow = false;
        }
    }

	void PlayGoController::sketchAction(QTabletEvent *event)
	{
		float pressure = event->pressure();
		QPointF sceneMappedPos = _view->mapToScene(event->pos());
		int time = 0;
		switch (event->type())
		{
		case QEvent::TabletPress :
			brushPress(sceneMappedPos, pressure, time);
			break;
		case QEvent::TabletMove :
			brushMove(sceneMappedPos, pressure, time);
			break;
		case QEvent::TabletRelease :
			brushRelease(sceneMappedPos, pressure, time);
			break;
		}
	}

	// \todo what is the eraser strength varies based on pressure value?
	void PlayGoController::eraseAction(QTabletEvent *event)
	{
		float pressure = event->pressure();
		QPointF sceneMappedPos = _view->mapToScene(event->pos());
		int time = 0;
		Point2DPT pos = Point2DPT(sceneMappedPos.x(),sceneMappedPos.y(), pressure, time);

		switch (event->type())
		{
		case QEvent::TabletPress :
			eraserPress(pos);
			break;
		case QEvent::TabletMove :
			eraserMove(pos);
			break;
		case QEvent::TabletRelease :
			eraserRelease(pos);
			break;
		}
	}

	void PlayGoController::selectAction(QTabletEvent *event)
	{
		float pressure = event->pressure();
		QPointF sceneMappedPos = _view->mapToScene(event->pos());
		int time = 0;
		Point2DPT pos = Point2DPT(sceneMappedPos.x(),sceneMappedPos.y(), pressure, time);

		switch (event->type())
		{
		case QEvent::TabletPress :
			lassoPress(pos);
			break;
		case QEvent::TabletMove :
			lassoMove(pos);
			break;
		case QEvent::TabletRelease :
			lassoRelease(pos);
			break;
		}
	}

	void PlayGoController::searchAction()
	{
		{
			// test display of QWebView

		}
		// We are searching if at least one of the strokes is highlighted
		if (!_itemHighlighted) return;
		// Do not use lasso polygon to find the selected strokes because selection can be done in
		// multiple iterations
		QImage inputImage =
//				_scene->getSelectionImage(_lassoPolygon);
				_scene->getSelectionImageFromHighlight();
		QList<Stroke*> selectedStrokes =
				_scene->getHighlightedStrokes();

		if (selectedStrokes.size() == 0) return;

		QList<SearchResult*> searchResults =
                _scene->page()->getSearchManager()->search(inputImage, 20);

        if (searchResults.size())
        {
			searchView->LoadSearchData(selectedStrokes, searchResults);
            emit onSearchComplete();
		} else
		{
			QMessageBox::about(_view,
							   QString("Search result"),
							   QString("No result found!"));
		}
	}

	// Slot functions
	void PlayGoController::setBrushWidth(int size)
	{
		_defaultPen.setWidth(size);
	}

	void PlayGoController::setBrushColor(QColor color)
	{
		_defaultPen.setColor(color);
	}

	void PlayGoController::onSearchBegin()
	{
		emit signalSearchBegin();
	}

	void PlayGoController::onSearchComplete()
	{
		emit signalSearchComplete();
	}

	void PlayGoController::onSearchItemSelect(SearchResult *result)
	{
		QList<Stroke*> selectedStrokes = _scene->getHighlightedStrokes();
		if (selectedStrokes.size())
		{
			QList<QGraphicsItem*> graphicsitems;
			for (int i=0; i < selectedStrokes.size(); i++)
				graphicsitems.push_back(selectedStrokes[i]);

			QRectF itemRect = _scene->getBoundingBox(graphicsitems);
			Component * newComponent = _page->createComponent();
			// Always set component position before adding objects to it
			newComponent->setTransform(QTransform().translate(itemRect.left(), itemRect.top()));

			for (int i=0; i < selectedStrokes.size(); i++)
			{
				newComponent->addToComponent(selectedStrokes[i]);
				selectedStrokes[i]->highlight(false);
				selectedStrokes[i]->hide();
			}
			Pixmap *pixmap = new Pixmap(result->resultFilePath);
			newComponent->addToComponent(pixmap);
			pixmap->setTransform(QTransform());
			pixmap->setPos(0,0);

			QRectF pixmapRect = pixmap->boundingRect();

			float scale = itemRect.width()/pixmapRect.width() < itemRect.height()/pixmapRect.height() ?
						itemRect.width()/pixmapRect.width() : itemRect.height()/pixmapRect.height();
			pixmap->setScale(scale);

			newComponent->recalculateBoundingRect();
		} else
		{
			// If selected strokes are empty.. this will create
			QMessageBox::about(_toplevelWindow, "Select search", "No stroke selected for replacing");
		}
	}

	void PlayGoController::onTabletEventFromView(QTabletEvent *event,
												 QGraphicsView *view)
	{
		//
		if (event->pointerType() == QTabletEvent::Pen)
		{
			switch (_activeMode)
			{
            case UI::Sketch :
				sketchAction(event);
				break;
            case UI::Erase :
				eraseAction(event);
				break;
            case UI::Select :
				selectAction(event);
				break;
			}
		} else if (event->pointerType() == QTabletEvent::Eraser)
		{
			if (_activeMode == UI::Sketch)	// Delete stroke
				eraseAction(event);
			if (_activeMode == UI::Select)
			{
				// delete all selected strokes
			}
		}
	}

	void PlayGoController::onMouseEventFromScene(QGraphicsSceneMouseEvent *mouseEvent, int status)
	{
		// NOTE - Do not count too much on these functions
		if (!_mouseModeEnabled) return;
		QPointF mousePos = mouseEvent->scenePos();
		Point2DPT pos = Point2DPT(mousePos.x(), mousePos.y(), 1.0f, 0);
		switch (_activeMode)
		{
        case UI::Sketch :
			switch (status)
			{
			case 0 :
				brushPress(pos);
				break;
			case 1:
				brushMove(pos);
				break;
			case 2 :
				brushRelease(pos);
				break;
			}
			break;
        case UI::Erase :
			switch (status)
			{
			case 0 :
				eraserPress(pos);
				break;
			case 1:
				eraserMove(pos);
				break;
			case 2 :
				eraserRelease(pos);
				break;
			}
			break;
        case UI::Select :
			switch (status)
			{
			case 0 :
				lassoPress(pos);
				break;
			case 1:
				lassoMove(pos);
				break;
			case 2 :
				lassoRelease(pos);
				break;
			}
			break;
		}
	}

	void PlayGoController::onTouchEventFromView(QTouchEvent *event, QGraphicsView *view)
	{
		// Single touch pan
//		if (event->touchPoints().count() == 1)
//		{
//			const QTouchEvent::TouchPoint &touchPoint1
//					= event->touchPoints().first();
//			QPointF currentPos =view->transform().map(touchPoint1.scenePos());
//			QPointF previousPos =view->transform().map(touchPoint1.lastScenePos());

//			view->translate(currentPos.x()-previousPos.x(),
//							currentPos.y()-previousPos.y());

//			view->translate((currentPos.x() - previousPos.x()),
//							(currentPos.y() - previousPos.y()));
//		}

//		// Two finger rotate and scale
//		if (event->touchPoints().count() == 2)
//		{
////			QList<QTouchEvent::TouchPoint> touchpoints = event->touchPoints();
//			const QTouchEvent::TouchPoint &tp1 = event->touchPoints()[0];
//			const QTouchEvent::TouchPoint &tp2 = event->touchPoints()[1];

//			// 1. Scaling by increase in radius
//			QPointF previousCenter = view->transform().map(0.5f* (tp1.lastScenePos() + tp2.lastScenePos()));
//			QPointF currentCenter = view->transform().map(0.5f* (tp1.scenePos() + tp2.scenePos()));

//			QPointF difference = currentCenter - previousCenter;

//			// 2. Translate by motion of center
//			// 3. Rotate by mean of rotation of each point by center
//		}
		if (event->touchPoints().count() == 3)
		{
			QList<QTouchEvent::TouchPoint> touchpoints = event->touchPoints();
			// 1. Scaling by increase in radius
			QPointF previousCenter = QPointF();
			QPointF currentCenter = QPointF();

			const QTouchEvent::TouchPoint &tp1 = touchpoints[0];
			const QTouchEvent::TouchPoint &tp2 = touchpoints[1];
			const QTouchEvent::TouchPoint &tp3 = touchpoints[2];

			foreach(QTouchEvent::TouchPoint touch, touchpoints)
			{
				previousCenter += touch.lastScenePos();
				currentCenter += touch.scenePos();
			}

			previousCenter = previousCenter /3;
			currentCenter = currentCenter /3;

			QPointF difference = view->transform().map(currentCenter) - view->transform().map(previousCenter);
			view->translate(difference.x(), difference.y());

			{
				//scaling
				QPointF a0 = tp1.lastScenePos(); QPointF a1 = tp1.scenePos();
				QPointF b0 = tp2.lastScenePos(); QPointF b1 = tp2.scenePos();
				QPointF c0 = tp3.lastScenePos(); QPointF c1 = tp3.scenePos();

				float d0 = diameterOfCircumcircle(euclideanDistance(&a0,&b0),
												  euclideanDistance(&b0,&c0),
												  euclideanDistance(&c0,&a0));

				float d1 = diameterOfCircumcircle(euclideanDistance(&a1,&b1),
												  euclideanDistance(&b1,&c1),
												  euclideanDistance(&c1,&a1));
				qDebug() << d0 << d1 << "SearchView";
				if ((qFuzzyCompare(d0,0) == false && qFuzzyCompare(d1,0)) == false)
				{
					view->scale(d1/d0,d1/d0);
				}

			}
			// 3-point rotate and scale are bit tricky to handle
		}
	}

	void PlayGoController::setToDraw()
	{
        setMode(UI::Sketch);
	}

	void PlayGoController::setToErase()
	{
        setMode(UI::Erase);
	}
	void PlayGoController::setToSelect()
	{
        setMode(UI::Select);
	}

	void PlayGoController::setToEdit()
	{
        setMode(UI::Edit);
	}

    void PlayGoController::setMode(UI::MODE newMode)
	{
        onModeChange(_activeMode, newMode);
        if (_activeMode == newMode)
        {
            return;
        }
		_activeMode = newMode;
	}

	void PlayGoController::enableMouse(bool enable)
	{
		_mouseModeEnabled = enable;
	}

	void PlayGoController::clearCurrentScene()
	{
		_page->deleteAll();
        _currentStroke = NULL;
	}

	void PlayGoController::drawMenusOnView(QPainter * painter, const QRectF & rect)
	{

	}

	void PlayGoController::onExternalImageAdd(const QString &path)
	{
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(_toplevelWindow,
									  "New image found",
									  "Load image" + path + "?",
									  QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			grabcut_process_image(this, path.toStdString(), "Cropped.png");
		}
	}

	void PlayGoController::loadImage(QString imagePath)
	{
		QPixmap _pixmap = QPixmap();
		_pixmap.load(imagePath);
		if (_pixmap.width() > 500 | _pixmap.height() > 500)
		{
			_pixmap = _pixmap.scaled(QSize(500,500), Qt::KeepAspectRatio);
		}
		Pixmap* pixmap = new Pixmap(_pixmap, imagePath);
		Component* component = _page->createComponent();
		component->addToComponent(pixmap);
	}

	void PlayGoController::startSimulation()
	{
		if (_page)
			_page->getPhysicsManager()->start(200);
	}

	void PlayGoController::pauseSimulation()
	{
		if (_page)
			_page->getPhysicsManager()->pause();
	}

	void PlayGoController::loadCamera()
	{

	}
}
