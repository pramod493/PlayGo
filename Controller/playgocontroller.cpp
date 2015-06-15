#include "PlayGoController.h"
#include <QObject>
#include <QDebug>
#include "cdiwindow.h"

namespace CDI
{
	PlayGoController::PlayGoController(SketchScene *scene, SketchView *view, CDIWindow *parent)
		:QObject(parent)
	{
		tree = new ModelViewTreeWidget(parent->playgo);
		tree->show();

		_toplevelWindow = parent;
		if (scene != NULL && view != NULL)
		{
			_scene = scene;
			_view = view;
			// Create all sort of connections
			// Connect this to the given view and scene objects for rendering and all other operations
			// connect(_scene, SIGNAL(),this, SLOT());

			connect(_view, SIGNAL(viewTabletEvent(QTabletEvent*,QGraphicsView*)),
					this, SLOT(onTabletEventFromView(QTabletEvent*,QGraphicsView*)));

			connect(_scene, SIGNAL(signalMouseEvent(QGraphicsSceneMouseEvent*,int)),
					this, SLOT(onMouseEventFromScene(QGraphicsSceneMouseEvent*,int)));
		}
		initController();
	}

	void PlayGoController::initController()
	{
		// Set the deafult value of other settings
		_mouseModeEnabled = true;
		_activeMode = MODE::Sketch;
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
			_currentComponent->component->setTransform(QTransform().translate
									   (pos.x(), pos.y()));
		}
		_currentStroke = _scene->addStroke(_currentComponent,
										   _defaultPen.color(), _defaultPen.widthF());


		_currentStroke->push_back(pos);
//		_currentStroke = new GraphicsPathItem(NULL, pos, pos.pressure(), pos.time());
//		_currentStroke->setPen(_defaultPen);
//		_currentStroke->setBrush(_defaultBrush);

		// TODO - Create a Scene::addStroke function
		// which automatically handles stroke addition
//		_scene->addItem(_currentStroke);

//		_currentStroke->parentStroke->setThickness(_defaultPen.widthF());
//		_currentStroke->parentStroke->setColor(_defaultPen.color());

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
		qDebug() << "Deleting" << selectedStrokes.size() << " items";
		foreach (GraphicsPathItem* stroke, selectedStrokes)
			delete (stroke->parentStroke);
		if (selectedStrokes.size()) _scene->update();

		qDebug() << "Looks fine";
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
		qDebug() << "Selection begin";
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
		_lasso->setPolygon(_lassoPolygon);
		_lasso->update(_lasso->boundingRect());

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
		if (_isLassoDisplayed)
		{
			if (_lassoPolygon.size())
			{
				// No transformation
				QImage inputImage = _scene->getSelectionImage(_lassoPolygon);
				inputImage.save ("Search_lasso_input.png");
			}
			_isLassoDisplayed = false;
			_lassoPolygon = QPolygonF();
			_lasso->setPolygon(QPolygonF());
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

	void PlayGoController::onTabletEventFromView(QTabletEvent *event,
												 QGraphicsView *view)
	{
		//
		if (event->pointerType() == QTabletEvent::Pen)
		{
			switch (_activeMode)
			{
			case MODE::Sketch :
				sketchAction(event);
				break;
			case MODE::Erase :
				eraseAction(event);
				break;
			case MODE::Select :
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
		case MODE::Sketch :
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
		case MODE::Erase :
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
		case MODE::Select :
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

	void PlayGoController::setToDraw()
	{
		setMode(MODE::Sketch);
	}

	void PlayGoController::setToErase()
	{
		setMode(MODE::Erase);
	}
	void PlayGoController::setToSelect()
	{
		setMode(MODE::Select);
	}

	void PlayGoController::setToEdit()
	{
		setMode(MODE::Edit);
	}

	void PlayGoController::setMode(MODE newMode)
	{
		_activeMode = newMode;
	}

	void PlayGoController::enableMouse(bool enable)
	{
		_mouseModeEnabled = enable;
	}

	void PlayGoController::clearCurrentScene()
	{
		_scene->page()->deleteAll();
	}
}
