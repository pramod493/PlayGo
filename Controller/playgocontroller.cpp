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
			view->setTransformationAnchor(QGraphicsView::NoAnchor);
			// Create all sort of connections
			// Connect this to the given view and scene objects for rendering and all other operations
			// connect(_scene, SIGNAL(),this, SLOT());

			connect(_view, SIGNAL(viewTabletEvent(QTabletEvent*,QGraphicsView*)),
					this, SLOT(onTabletEventFromView(QTabletEvent*,QGraphicsView*)));

			connect(_scene, SIGNAL(signalMouseEvent(QGraphicsSceneMouseEvent*,int)),
					this, SLOT(onMouseEventFromScene(QGraphicsSceneMouseEvent*,int)));

			connect(_view, SIGNAL(viewTouchEvent(QTouchEvent*,QGraphicsView*)),
					this, SLOT(onTouchEventFromView(QTouchEvent*,QGraphicsView*)));

			connect(_view, SIGNAL(viewDrawforeground(QPainter*,const QRectF&)),
					this, SLOT(drawMenusOnView(QPainter*,const QRectF&)));
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
		_currentComponent = NULL;

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
		// Carry out the actual rendering function
		if (_currentStroke != NULL)	delete _currentStroke;
		if (_currentComponent == NULL)
		{
			_currentComponent = _scene->addComponent();
			QTransform t = _currentComponent->component->transform();
			t = t.translate(pos.x(), pos.y());
			t = t.rotate(30);
			_currentComponent->component->setTransform(t);
		}
		_currentStroke = _scene->addStroke(_currentComponent,
										   _defaultPen.color(), _defaultPen.widthF());
		_currentStroke->push_back(pos);

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
		if (!(_currentStroke == NULL) && _isDrawingNow)
			_currentStroke->push_back(pos);
//		_currentStroke->update(_currentStroke->boundingRect());
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
		if (!(_currentStroke == NULL) && _isDrawingNow)
		{
			_currentStroke->push_back(pos);
			_currentStroke->ApplySmoothing(2);
			emit strokeComplete(_currentStroke);
			// TODO Apply smoothing before the object is updated maybe?
			_isDrawingNow = false;

			_currentStroke = NULL;
			_scene->update();
		}
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
		QList<GraphicsPathItem*> selectedStrokes = _scene->getSelectedStrokes(Point2D(pos.x(),pos.y()), _defaultPen.widthF());
		QLOG_INFO() << "Deleting" << selectedStrokes.size() << " items";
		foreach (GraphicsPathItem* stroke, selectedStrokes)
        {
			Stroke* s = stroke->parentStroke();
			if (s == NULL) continue;
			_scene->onItemRemove(s->id());
			delete s;
        }
//		if (selectedStrokes.size()) _scene->update();
		// \todo - Implement related functions in scene
		//		QList<GraphicsPolygon2D*> selectedPolygons = _scene->getSelectedPoly(pos);
		//		QList<QGraphicsPathItem*> selectedItems = _scene->getMiscSelection(pos);
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
		_lasso->setPolygon(QPolygonF());
		_lasso->update(_lasso->boundingRect());

        QList<GraphicsPathItem*> selectedStrokes = _scene->getSelectedStrokes(_lassoPolygon, 1.0f);
        if (selectedStrokes.size()) _itemHighlighted = true;
        for (int i=0; i < selectedStrokes.size(); i++)
        {
            selectedStrokes[i]->highlight(true);
        }
		_isLassoDisplayed = false;
	}

    void PlayGoController::onModeChange(UI::MODE oldmode, UI::MODE newmode)
    {
        if (oldmode == UI::Select)
        {
            if (_itemHighlighted)
                _scene->clearHighlight();
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
		// We are searching if at least one of the strokes is highlighted
		if (!_itemHighlighted) return;
		// Do not use lasso polygon to find the selected strokes because selection can be done in
		// multiple iterations
		QImage inputImage =
				_scene->getSelectionImage(_lassoPolygon);
//				_scene->getSelectionImageFromHighlight();
		QList<GraphicsPathItem*> selectedStrokes =
				_scene->getHighlightedStrokes();

		if (selectedStrokes.size() == 0) return;

		inputImage.save("highlight_selection.png");
		return;
		// NOTE - What is going on here??
        for (int i=0; i < selectedStrokes.size(); i++)
        {
			selectedStrokes[i]->highlight(true);
        }
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

	void PlayGoController::onSearchTrigger()
	{
		emit searchTriggered();
	}

	void PlayGoController::onSearchComplete()
	{
		emit searchCompleted();
	}

	void PlayGoController::onSearchItemSelect(SearchResult *result)
	{
		qDebug() << "Selected";
		QMessageBox::about(NULL, "TITLE", "ITEM SELECTED");
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
			eraseAction(event);
		}
	}

	void PlayGoController::onMouseEventFromScene(QGraphicsSceneMouseEvent *mouseEvent, int status)
	{
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
		if (event->touchPoints().count() == 1)
		{
			const QTouchEvent::TouchPoint &touchPoint1
					= event->touchPoints().first();
			QPointF currentPos =view->transform().map(touchPoint1.scenePos());
			QPointF previousPos =view->transform().map(touchPoint1.lastScenePos());

			view->translate(currentPos.x()-previousPos.x(),
							currentPos.y()-previousPos.y());

			QLOG_INFO() << "View move by " << (currentPos.x() - previousPos.x());
			view->translate((currentPos.x() - previousPos.x()),
							(currentPos.y() - previousPos.y()));
		}
		if (event->touchPoints().count() == 3)
		{
			QList<QTouchEvent::TouchPoint> touchpoints = event->touchPoints();
			// 1. Scaling by increase in radius
			QPointF previousCenter = QPointF();
			QPointF currentCenter = QPointF();
			foreach(QTouchEvent::TouchPoint touch, touchpoints)
			{
				previousCenter += touch.lastScenePos();
				currentCenter += touch.scenePos();
			}

			QPointF difference = currentCenter - previousCenter;

			// 2. Translate by motion of center
			// 3. Rotate by mean of rotation of each point by center

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
        _scene->clearHighlight();
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
            // No change of mode required
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
		_scene->page()->deleteAll();
        _scene->clear();
        _currentComponent = NULL;
        _currentStroke = NULL;
	}

	void PlayGoController::drawMenusOnView(QPainter * painter, const QRectF & rect)
	{

	}
}
