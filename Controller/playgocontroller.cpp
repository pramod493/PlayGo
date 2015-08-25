#include <QApplication>
#include "PlayGoController.h"
#include "commonfunctions.h"
#include <QLabel>
#include "QsLog.h"

#include "cdiwindow.h"
#include "cdisearchgraphicsitem.h"
#include "SelectableActions.h"

#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsColorizeEffect>
#include <QGraphicsBlurEffect>
#include <QPropertyAnimation>

#include "touchandholdcontroller.h"
#include "mainsettings.h"

#include "cdiboxdebugdraw.h"
#include <QDockWidget>

namespace CDI
{

PlayGoController::PlayGoController(SketchView *view, CDIWindow *parent)
	:QObject(parent)
{
	if (view == NULL || parent == NULL)
	{
		QLOG_FATAL() << "Invalid scene and view reference.";
		QMessageBox::about(NULL, "Fatal error!",
						   "Cannot initialize controller. Invalid arguments in the constructor.\n Exiting now...");
		QApplication::quit();
	}

	// Display the model tree as one of the docks
	// tree = new ModelViewTreeWidget(parent->playgo);
	// tree->show();
	// QDockWidget* treeDock = new QDockWidget(parent);
	// treeDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea |
	// 					  Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	// treeDock->setWidget(tree);
	tree = nullptr;

	_toplevelWindow = parent;
	_scene = static_cast<SketchScene*>(view->getPage()->scene());
	_view = view;
	_viewport = view->viewport();
	_page = view->getPage();

	// Do not install event filter on QGraphicsView but on viewport
	// Installing event on QGraphicsView seems to filter out touch
	_view->viewport()->installEventFilter(this);

	// Customize settings of view
	_view->setTransformationAnchor(QGraphicsView::NoAnchor);
	_view->setOptimizationFlag(QGraphicsView::DontSavePainterState);
	_view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

	// Allows drawing on background as well as foreground of view
	connect(_view, SIGNAL(viewDrawforeground(QPainter*,const QRectF&)),
			this, SLOT(drawMenusOnView(QPainter*,const QRectF&)));

	connect(_view, SIGNAL(viewImageDrop(QString, QObject*, QDropEvent*)),
			this, SLOT(loadImage(QString, QObject*, QDropEvent*)));

	initController();

	// We are not using the SearchView any more for displaying search results
	searchItemGroup = nullptr;
	_searchResultsDisplayed = false;
	// Triggers the closing of search results
	hideSearchResultAction = new QAction
			(QIcon(":/images/overlay/close-02.png"), tr("Hide search results"), this);
	connect(hideSearchResultAction, SIGNAL(triggered()),
			this, SLOT(closeSearchResultDisplay()));

	QTapAndHoldGesture::setTimeout(2000);
}

PlayGoController::~PlayGoController()
{
	if (tree != NULL) delete tree;
}

void PlayGoController::initController()
{
	// Set the deafult value of other settings

	// Drawing parameters
	_activeMode		= UI::Sketch;
	_lastActiveMode	= _activeMode;
	_device = QTabletEvent::NoDevice;

	_defaultPen = QPen(Qt::black);
	_defaultPen.setCapStyle(Qt::RoundCap);
	_defaultPen.setJoinStyle(Qt::RoundJoin);
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

	_fillBrush = QBrush(QColor(250,150,150,200), Qt::SolidPattern);
	_lassoBrush = QBrush(Qt::NoBrush);
	_highlightBrush = QBrush(Qt::NoBrush);

	_isDrawingStroke = false;
	_currentStroke = NULL;
	_currentPolygon = NULL;

	// Lasso parameters
	_lasso = new QGraphicsPolygonItem();
	_lasso->setPen(_lassoPen);
	_lasso->setBrush(_lassoBrush);
	_scene->addItem(_lasso);
	_isLassoDisplayed = false;

	_itemHighlighted = false;

	// Physics settings.
	_physicsMask = 0;

	///////////////////////////////////////////////////////
	// Connection option settings
	createConnectionsToolbar();
	hideConnectionsToolbar();
	_activeConnectionMode = HingeJoint;

	// Gesture recognizer stroke
	_tmpStrokes = QList<PenStroke*>();
	_currentConnectStroke = NULL;

	// Force line
	forceLine = NULL;

	// Slider variables
	_sliderComponentA = NULL;
	_sliderComponentB = NULL;
	_sliderStartPos = QPointF();
	_sliderEndPos = QPointF();
	_sliderLineItem = NULL;


	sketchRecognizer = new PDollarRecognizer(this);		// Will make sure that this gets deleted with the object
	QString gestureDir = getHomeDirectory() + "/gestures/";
	sketchRecognizer->loadPDRTemplates(gestureDir);

	//////////////////////////////////////////////////////////
	// Tap and hold options
	_tapOverrideEnabled = false;
	touchholdController = new TouchAndHoldController(this);
	touchholdController->setMainController(this);

	/////////////////////////////////////////////////////////////////
	_isStylusNearby = false;

	//////////////////////////////////////////////////////////////////
	_searchResultsDisplayed = false;
}

void PlayGoController::brushPress(QPointF pos, float pressure, int time)
{
	_currentStroke = new Stroke();
	_currentStroke->setPos(pos);
	_currentStroke->setPen(_defaultPen);
	_currentStroke->setBrush(_defaultBrush);

	_currentStroke->push_point(Point2DPT(pos.x(),pos.y(),pressure, time));
	_scene->addItem(_currentStroke);
	_isDrawingStroke = true;
}

void PlayGoController::brushMove(QPointF pos, float pressure, int time)
{
	if (_page == NULL) return;
	if (!(_currentStroke == NULL) && _isDrawingStroke)
		_currentStroke->push_point(Point2DPT(pos.x(),pos.y(),pressure, time));
}

void PlayGoController::brushRelease(QPointF pos, float pressure, int time)
{
	if (_page == NULL || _currentStroke == NULL || !_isDrawingStroke) return;

	_isDrawingStroke = false;
	_currentStroke->push_point(Point2DPT(pos.x(),pos.y(),pressure, time));
	_currentStroke->applySmoothing(2);

	// Do the addition to the component here
	if (_page->currentComponent() == NULL)
	{
		Component* currentComponent = _page->createComponent();
		QTransform t;
		t = t.translate(pos.x(), pos.y());
		currentComponent->setTransform(t);
		_page->setCurrentComponent(currentComponent);
		_page->currentComponent()->setZValue(Z_COMPONENTVIEW);
	}

	// Add the stroke to the compopnent
	_page->currentComponent()->addToComponent(_currentStroke);

	// recalculate the item's bounding rect
	if (_page->currentComponent())_page->currentComponent()->recalculateBoundingRect();

	// Set the current stroke's value to null
	_currentStroke = nullptr;
}

void PlayGoController::shapePress(QPointF pos)
{
	Component *newComponent = _page->createComponent();
	_currentPolygon = new Polygon2D();
	newComponent->addToComponent(_currentPolygon);
	//		_currentPolygon->setPos(0,0);
	newComponent->moveBy(pos.x(),pos.y());
	newComponent->setOpacity(0.75f);
	_currentPolygon->setOpacity(0.90f);
	_currentPolygon->push_point(pos);

	_fillPen = _defaultPen;
	_fillPen.setWidth(3);
	QColor tmpColor = _fillPen.color().lighter();
	_fillBrush.setColor(tmpColor);
	_currentPolygon->setPen(_fillPen);
	_currentPolygon->setBrush(_fillBrush);
	_isDrawingPolygon = true;
}

void PlayGoController::shapeMove(QPointF pos)
{
	if (_currentPolygon && _isDrawingPolygon)
		_currentPolygon->push_point(pos);
}

void PlayGoController::shapeRelease(QPointF pos)
{
	if (_currentPolygon && _isDrawingPolygon)
	{
		_currentPolygon->push_point(pos);
		_currentPolygon->initializePhysicsShape();
		if (_currentPolygon->parentItem())
		{
			if (_currentPolygon->parentItem()->type() == Component::Type)
			{
				Component *component = qgraphicsitem_cast<Component*>(_currentPolygon->parentItem());
				component->recalculateBoundingRect();
				component->internalTransformChanged();
				component->requiresRegeneration = true;
				component->setOpacity(1.0f);
			} else
			{
				_currentPolygon->parentItem()->setOpacity(1.0f);
			}
		}
	}
	_toplevelWindow->colorToolbar->slotRandomizeColor();	// randomizes the main color after each selection
	_currentPolygon = NULL;
	_isDrawingPolygon = false;
}

void PlayGoController::eraserPress(QPointF pos)
{
	eraserMove(pos);
}

void PlayGoController::eraserMove(QPointF pos)
{
	if (_activeMode == UI::Sketch)
	{
		QList<Stroke*> selectedStrokes = _page->getSelectedStrokes(Point2D(pos.x(),pos.y()), _defaultPen.widthF());
		for (int i=0; i< selectedStrokes.size(); i++)
			delete selectedStrokes[i];
	} else
	{
		QList<QGraphicsItem*> selectedItems = _page->getSelectedItems(Point2D(pos.x(),pos.y()), _defaultPen.widthF());
		QList<QGraphicsItem*>::iterator iter;
		for (iter = selectedItems.begin(); iter != selectedItems.end();
			 ++iter)
		{
			QGraphicsItem* selectedItem = *(iter);
			if (selectedItem->type() == Component::Type)
			{
				_page->destroyComponent(qgraphicsitem_cast<Component*>(selectedItem));
			} else {
				delete selectedItem;
			}
		}
	}
}

void PlayGoController::eraserRelease(QPointF pos)
{
	eraserMove(pos);
}

void PlayGoController::lassoPress(QPointF pos)
{
	_isLassoDisplayed = true;
	_lassoPolygon = QPolygonF();
	_lassoPolygon.push_back(pos);
	_lasso->setPolygon(_lassoPolygon);
	_lasso->update(_lasso->boundingRect());
}

void PlayGoController::lassoMove(QPointF pos)
{
	_lassoPolygon.push_back(pos);
	_lasso->setPolygon(_lassoPolygon);
	_lasso->update(_lasso->boundingRect());
}

void PlayGoController::lassoRelease(QPointF pos)
{
	_lassoPolygon.push_back(pos);
	//_lasso->setPolygon(_lassoPolygon);
	_lasso->update(_lasso->boundingRect());

	QList<Stroke*> selectedStrokes = _page->getSelectedStrokes(_lassoPolygon, 1.0f);
	if (selectedStrokes.size()) _itemHighlighted = true;
	for (int i=0; i < selectedStrokes.size(); i++)
	{
		selectedStrokes[i]->highlight(true);
	}
	_lasso->setPolygon(QPolygonF());
	_isLassoDisplayed = false;
}

void PlayGoController::connectPress(QPointF scenePos)
{
	// update all pending component positions so that physics engine works on
	// the updated configuration
	_page->onSimulationStepStart();
	switch (_activeConnectionMode)
	{
	case GestureSketch :
		gestureSketchModeFilter(scenePos, UI::Began);
		break;
	case StaticJoint :
		staticJointModeFilter(scenePos, UI::Began);
	case HingeJoint :
		hingeJointModeFilter(scenePos, UI::Began);
		break;
	case SliderJoint :
		sliderJointModeFilter(scenePos, UI::Began);
		break;
	case SpringJoint :
		break;		// Nothing to do for spring joint
	case ApplyForce :
		forceModeFilter(scenePos, UI::Began);
		break;
	}
}

void PlayGoController::connectMove(QPointF scenePos)
{
	switch (_activeConnectionMode)
	{
	case GestureSketch :
		gestureSketchModeFilter(scenePos, UI::Update);
		break;
	case StaticJoint :
		staticJointModeFilter(scenePos, UI::Update);
		break;
	case HingeJoint :
		hingeJointModeFilter(scenePos, UI::Update);
		break;
	case SliderJoint :
		sliderJointModeFilter(scenePos, UI::Update);
		break;
	case SpringJoint :
		break;
	case ApplyForce :
		forceModeFilter(scenePos, UI::Update);
		break;
	}
}

void PlayGoController::connectRelease(QPointF scenePos)
{
	_page->onSimulationStepStart();	// Update all pending updates
	switch (_activeConnectionMode)
	{
	case GestureSketch :
		gestureSketchModeFilter(scenePos, UI::End);
		break;
	case StaticJoint :
		staticJointModeFilter(scenePos, UI::End);
		break;
	case HingeJoint :
		hingeJointModeFilter(scenePos, UI::End);
		break;
	case SliderJoint :
		sliderJointModeFilter(scenePos, UI::End);
		break;
	case SpringJoint :
		break;
	case ApplyForce :
		forceModeFilter(scenePos, UI::End);
		break;
	default:
		break;	// Nothing to do
	}
}

void PlayGoController::gestureSketchModeFilter(QPointF scenePos, UI::EventState eventState)
{
	switch (eventState)
	{
	case UI::Began :
	{
		if (_currentConnectStroke)
		{
			QLOG_WARN() << "Current stroke should have been NULL.";
			delete _currentConnectStroke;
			_currentConnectStroke = NULL;
		}

		_currentConnectStroke = new PenStroke();
		_scene->addItem(_currentConnectStroke);
		_currentConnectStroke->push_point(scenePos);	// No transformation for this one
		QPen strokePen = QPen();
		strokePen.setWidth(3);
		strokePen.setColor(Qt::blue);
		_currentConnectStroke->setPen(strokePen);
		break;
	}
	case UI::Update :
	{
		if (_currentConnectStroke)
			_currentConnectStroke->push_point(scenePos);
		break;
	}
	case UI::End :
	{
		if (_currentConnectStroke == NULL) break;
		_currentConnectStroke->push_point(scenePos);

		sketchRecognizer->addStroke(_currentConnectStroke->points);
		_tmpStrokes.push_back(_currentConnectStroke);
		sketchRecognizer->gbRecognize();

		_currentConnectStroke = NULL;
		break;
	}
	case UI::Cancel :
	{
		// Delete the stroke
	}
	}
}

void PlayGoController::staticJointModeFilter(QPointF scenePos, UI::EventState eventState)
{
	switch(eventState)
	{
	case UI::Began :
	{
		break;
	}
	case UI::Update :
	{
		break;
	}
	case UI::End :
	{
		QList<Component*> components = getSelectableComponentsByPhysics(scenePos);
		if (components.size() == 0) break;

		Component* component = components[0];
		if (component->isStatic())
			component->setStatic(false);
		else
			component->setStatic(true);
		break;
	}
	case UI::Cancel :
	{
		break;
	}
	}
}

void PlayGoController::hingeJointModeFilter(QPointF scenePos, UI::EventState eventState)
{
	switch(eventState)
	{
	case UI::Began :
	{
		break;
	}
	case UI::Update :
	{
		break;
	}
	case UI::End :
	{
		/*QList<Component*>*/ auto itemsToUse = getSelectableComponentsByPhysics(scenePos);

		if (itemsToUse.size() < 2) break;
		// Ignore all other components under selection
		Component *c1 = itemsToUse[0];
		Component *c2 = itemsToUse[1];

		if (c1->id() == c2->id())
		{
			if (itemsToUse.size() > 2)
			{
				c2 = itemsToUse[2];
			}
		}

		bool b_enableMotor = false;
		float f_motorSpeed = 0;
		float f_motorTorque = 0;
		getMotorParams(&b_enableMotor, &f_motorSpeed, &f_motorTorque);
		if (b_enableMotor)	// Disable motor after first pin joint is created
			setMotorParams(false, f_motorSpeed, f_motorTorque);

		qDebug() << b_enableMotor << f_motorSpeed << f_motorTorque;
		auto physicsJoint = _page->getPhysicsManager()->createPinJoint(
					c1, c2, scenePos,
					b_enableMotor, false,
					f_motorSpeed, f_motorTorque,
					0, 0);

		QLOG_INFO() << "Pin joint created w/" << physicsJoint->id().toString();

//		// Already set. Doesn't hurt to do so again
//		physicsJoint->setPos(c1->mapFromScene(scenePos));

//		// Disable motor because we are not in the play mode. Motor might ruin things
//		if (b_enableMotor)
//		{
//			b2RevoluteJoint *revoluteJoint = static_cast<b2RevoluteJoint*>(physicsJoint->getcdjoint()->joint());
//			revoluteJoint->EnableMotor(false);
//		}
		break;
	}
	case UI::Cancel :
	{
		break;
	}
	}
}

void PlayGoController::sliderJointModeFilter(QPointF scenePos, UI::EventState eventState)
{
	switch(eventState)
	{
	case UI::Began :
	{
		if (_sliderComponentA && _sliderComponentB)
		{
			_sliderStartPos = scenePos;
			if (_sliderLineItem) delete _sliderLineItem;
			_sliderLineItem = new QGraphicsLineItem(QLineF(_sliderStartPos, _sliderStartPos));
			_sliderLineItem->setZValue(Z_UIVIEW);		// used for representing display
			_scene->addItem(_sliderLineItem);
		}
		break;
	}
	case UI::Update :
	{
		if (_sliderLineItem)
		{
			_sliderEndPos = scenePos;
			_sliderLineItem->setLine(QLineF(_sliderStartPos, _sliderEndPos));
		}
		break;
	}
	case UI::End :
	{
		if (_sliderComponentA == nullptr)
		{
			QList<Component*> selection = getSelectableComponentsByPhysics(scenePos);
			if (selection.size())
			{
				_sliderComponentA = selection[0];
				_sliderComponentA->setOpacity(1.0f);
			}
		} else if (_sliderComponentB == nullptr)
		{
			QList<Component*> selection = getSelectableComponentsByPhysics(scenePos);
			if (selection.size())
			{
				if (_sliderComponentA == selection[0])
				{
					if (selection.size() > 1)
					{
						_sliderComponentB = selection[1];
						_sliderComponentB->setOpacity(1.0f);
					}
				} else
				{
					_sliderComponentB = selection[0];
					_sliderComponentB->setOpacity(1.0f);
				}
			}
		} else
		{
			_sliderEndPos = scenePos;
			if (false)	// draw spring shapes
			{
				QGraphicsPathItem* pathitem = new QGraphicsPathItem;
				QPainterPath path;
				int width = 30;
				QLineF line = QLineF(_sliderStartPos, _sliderEndPos);
				int n = static_cast<int>(line.length()/width);
				line.setLength(width);
				QPointF movePos = line.p2() - line.p1();
				QLineF normal = line.normalVector();
				normal.setLength(width);
				QPointF normalPos = normal.p2() - normal.p1();

				path.moveTo(_sliderStartPos);
				for (int i=0; i < n; i++)
				{
					path.lineTo(line.x2()+normalPos.x(), line.y2()+normalPos.y());
					line.translate(movePos);
					path.lineTo(line.x2(), line.y2());
				}
				pathitem->setPath(path);

				QPen linePen = QPen(Qt::red);
				linePen.setWidth(3);
				pathitem->setPen(linePen);
				_scene->addItem(pathitem);
			}

			bool b_enableMotor = false;
			float f_motorSpeed = 0;
			float f_motorForce = 0;
			getMotorParams(&b_enableMotor, &f_motorSpeed, &f_motorForce);
			if (b_enableMotor)
				setMotorParams(false, f_motorSpeed, f_motorForce);

			auto physicsJoint = _page->getPhysicsManager()->createPrismaticJoint(
						_sliderComponentA, _sliderComponentB,
						_sliderStartPos, _sliderEndPos,
						b_enableMotor, true,
						f_motorSpeed, f_motorForce);	// Keep limits same ass start and end pos
						//0, vectorGlobal.length());

			QLOG_INFO() << "Slider joint created with ID:" << physicsJoint->id();

			_sliderComponentA->setOpacity(0.5f);
			_sliderComponentB->setOpacity(0.5f);

			if (_sliderLineItem) delete _sliderLineItem;
			_sliderLineItem = nullptr;

			_sliderComponentA = nullptr;
			_sliderComponentB = nullptr;
		}
		break;
	}
	case UI::Cancel :
	{
		// Delete/reset eveything
		if (_sliderLineItem) delete _sliderLineItem;
		_sliderLineItem = nullptr;
		_sliderComponentA = nullptr;
		_sliderComponentB = nullptr;
		break;
	}
	}
}

void PlayGoController::forceModeFilter(QPointF scenePos, UI::EventState eventState)
{
	switch (eventState)
	{
	case UI::Began :
	{
		// TODO - Default to application of force
		/*QList<QGraphicsItem*>*/ auto selections = _scene->items(scenePos,
														 Qt::IntersectsItemBoundingRect);
		if (selections.size() == 0) break;

		//foreach(QGraphicsItem* graphicsitem, selections)
		for(auto graphicsitem : selections)
		{
			if (graphicsitem->type() != Pixmap::Type) continue;
			// Must have a component attached
			if (graphicsitem->parentItem() == NULL) continue;
			// Pixmap must be of type component
			if (graphicsitem->parentItem()->type() != Component::Type) continue;

			Pixmap *pixmap = qgraphicsitem_cast<Pixmap*>(graphicsitem);
			// We will use the shape for later stages
			if (pixmap->contains(pixmap->mapFromScene(scenePos)) == false) continue;
			Component* component
					= qgraphicsitem_cast<Component*>(graphicsitem->parentItem());
			QTransform t = QTransform::fromTranslate(scenePos.x(), scenePos.y());
//			forceLine = new ForceGraphicsItem(0,0,0,0);
//			forceLine->setTransform(t);
//			_scene->addItem(forceLine);
//			component->addToComponent(forceLine);
			return;
		}
		break;
	}
	case UI::Update :
	{
		if (forceLine)
		{
			QPointF tPos = forceLine->mapFromScene(scenePos);
			forceLine->setLine(0, 0, tPos.x(), tPos.y());
		}
		break;
	}
	case UI::End :
	{
		if (forceLine == NULL)
		{
			QLOG_ERROR() << "ERROR!! forceLine should not be NULL.";
			return;
		}
		QPointF tPos = forceLine->mapFromScene(scenePos);
		forceLine->setLine(0,0,tPos.x(),tPos.y());

		if (forceLine->parentItem())
		{
			if (forceLine->parentItem()->type() == Component::Type)
			{
				Component* parentComponent = qgraphicsitem_cast<Component*>(forceLine->parentItem());
				if (parentComponent->physicsBody())
				{
					QPointF forceorigin = forceLine->mapToParent(QPointF(0,0));
					QPointF forcevector = forceorigin - forceLine->mapToParent(tPos);
					/*float forceScale = 1.0f;
						float physicsScale = getPhysicsScale();
						parentComponent->physicsBody()->ApplyForce
								(b2Vec2(forcevector.x()/forceScale, forcevector.y()/forceScale),
								 b2Vec2(forceorigin.x()/physicsScale, forceorigin.y()/physicsScale),
								 true);
						*/
					parentComponent->physicsBody()->SetLinearVelocity(b2Vec2(forcevector.x(), forcevector.y()));

					QLOG_INFO() << "Force output" << forceorigin << forcevector;
				}
			}
		}
		delete forceLine;
		forceLine = NULL;
		break;
	}
	case UI::Cancel :
		break;
	}
}

void PlayGoController::createConnectionsToolbar()
{
	QActionGroup * actionGroup = new QActionGroup(this);
	QAction* scribbleModeSelection = new QAction(QIcon(":/images/scribble.png"), tr("Recognizer mode"), actionGroup);
	scribbleModeSelection->setCheckable(true);
	scribbleModeSelection->setChecked(false);

	QAction* lockItemAction = new QAction(QIcon(":/images/joints/lock-joint.png"), tr("Fix item"), actionGroup);
	lockItemAction->setCheckable(true);
	lockItemAction->setChecked(false);

	QAction* hingeSelectAction = new QAction(QIcon(":/images/joints/hinge-joint.png"), tr("Create hinge joint"), actionGroup);
	hingeSelectAction->setCheckable(true);
	hingeSelectAction->setChecked(true);

	QAction* sliderSelectAction = new QAction(QIcon(":/images/joints/slider.png"), tr("Create slider joint"), actionGroup);
	sliderSelectAction->setCheckable(true);
	sliderSelectAction->setChecked(false);

	QAction* springSelectAction = new QAction(QIcon(":/images/joints/spring-joint.png"), tr("Connect using springs"), actionGroup);
	springSelectAction->setCheckable(true);
	springSelectAction->setChecked(false);

	QAction* forceSelectAction = new QAction(QIcon(":/images/joints/force.png"), tr("Apply force"), actionGroup);
	forceSelectAction->setCheckable(true);
	forceSelectAction->setChecked(false);

	connectionOptionsToolbar = new QToolBar(QString("Connect mode toolbar"), _toplevelWindow);

	connectionOptionsToolbar->addAction(scribbleModeSelection);
	connectionOptionsToolbar->addAction(lockItemAction);
	connectionOptionsToolbar->addAction(hingeSelectAction);
	connectionOptionsToolbar->addAction(sliderSelectAction);
	connectionOptionsToolbar->addAction(springSelectAction);
	connectionOptionsToolbar->addAction(forceSelectAction);

	connectionOptionsToolbar->setIconSize(QSize(48,48));

	_toplevelWindow->addToolBar(Qt::TopToolBarArea, connectionOptionsToolbar);

	connect(scribbleModeSelection, SIGNAL(triggered()),
			this, SLOT(setModeScribble()));
	connect(lockItemAction, SIGNAL(triggered()),
			this, SLOT(setModeLockItem()));
	connect(hingeSelectAction, SIGNAL(triggered()),
			this, SLOT(setModeHingeJoint()));
	connect(sliderSelectAction, SIGNAL(triggered()),
			this, SLOT(setModeSliderJoint()));
	connect(springSelectAction, SIGNAL(triggered()),
			this, SLOT(setModeSpringJoint()));
	connect(forceSelectAction, SIGNAL(triggered()),
			this, SLOT(setModeForce()));

	/////---------------------------
	/// Create entry boxes
	connectionOptionsToolbar->addSeparator();
	enableMotorCheckbox = new QCheckBox(QString("Enable motor"));
	connectionOptionsToolbar->addWidget(enableMotorCheckbox);
	enableMotorCheckbox->setChecked(true);
	connectionOptionsToolbar->addSeparator();

	motorSpeed = new QLineEdit();
	QValidator *speedvalidator = new QIntValidator(-500,5000, motorSpeed);
	motorSpeed->setValidator(speedvalidator);
	QLabel *speedlabel = new QLabel(QString("Motor speed:"));
	connectionOptionsToolbar->addWidget(speedlabel);
	connectionOptionsToolbar->addWidget(motorSpeed);
	motorSpeed->setText(QString("100"));

	connectionOptionsToolbar->addSeparator();

	motorTorque = new QLineEdit();
	QValidator *torqueValidator = new QIntValidator(-10000,10000, motorTorque);
	motorTorque->setValidator(torqueValidator);
	QLabel *torqueLabel = new QLabel(QString("Torque"));
	connectionOptionsToolbar->addWidget(torqueLabel);
	connectionOptionsToolbar->addWidget(motorTorque);
	motorTorque->setText(QString("150"));
}

void PlayGoController::showConnectionsToolbar()
{
	if (connectionOptionsToolbar) connectionOptionsToolbar->show();
}

void PlayGoController::hideConnectionsToolbar()
{
	if (connectionOptionsToolbar) connectionOptionsToolbar->hide();
}

void PlayGoController::getMotorParams(bool* motorEnable, float *speed, float *torque)
{
	if (connectionOptionsToolbar)
	{
		*motorEnable = enableMotorCheckbox->isChecked();
		bool ok;
		float f_speed = motorSpeed->text().toFloat(&ok);
		if (ok) *speed = f_speed;		//revolutJointDef.motorSpeed = speed * 2.0f *3.14f;
		float f_torque= motorTorque->text().toFloat(&ok);
		if (ok)	 *torque = f_torque;	//revolutJointDef.maxMotorTorque = torque;
	}
}

void PlayGoController::setMotorParams(bool motorEnable, float speed, float torque)
{
	if (connectionOptionsToolbar)
	{
		enableMotorCheckbox->setChecked(motorEnable);
		motorSpeed->setText(QString::number(speed));
		motorTorque->setText(QString::number(torque));
	}
}

bool PlayGoController::onModeChange(UI::MODE oldmode, UI::MODE newmode)
{
	bool retval = true;
	if (oldmode == UI::Select)
	{
		if (newmode == UI::Erase) // delete highlight
		{
			QList<Stroke*> highlihghtedItems = _page->getHighlightedStrokes();
			for (int i=0; i < highlihghtedItems.size(); i++)
				delete highlihghtedItems[i];
		}
		if (_itemHighlighted)
				_page->clearStrokeHighlight();
		_isLassoDisplayed = false;
		_lassoPolygon.clear();
		_lasso->setPolygon(_lassoPolygon);
	}

	if (oldmode == UI::Sketch)
	{
		_currentStroke = NULL;
				_isDrawingStroke = false;
	}

	if (oldmode == UI::Connect)
	{
		QList<Component*> components = _page->getComponents();
		foreach (Component* component, components)
			component->setOpacity(1.0f);
		hideConnectionsToolbar();
		connectionModeReset();
	}

	if (newmode == UI::Connect)
	{
		QList<Component*> components = _page->getComponents();
		foreach (Component* component, components)
			component->setOpacity(0.75f);
		showConnectionsToolbar();
		connectionModeReset();

		// In UI connect mode, keep physics engine ON but disable many options
		PhysicsManager* physicsManager = _page->getPhysicsManager();
		physicsManager->start(100);	// keep it super slow. Do not hamper the interactions
		physicsManager->setEnableGravity(false);
		physicsManager->setEnableMotor(false);

	}

	if (newmode == UI::None)
	{
		// In this case we are mostly entering the
		// physics state whether play/pause

	}
	return retval;
}

bool PlayGoController::eventFilter(QObject *obj, QEvent *event)
{
	if (obj != _viewport) return false;

	auto eventType = event->type();

	// Test out auto generated mouse events
	switch (eventType)
	{
	case QEvent::MouseButtonDblClick :
		qDebug() << "Double click detected @ filtering" << "use only if from touch";
		break;
	case QEvent::MouseButtonPress :
	case QEvent::MouseButtonRelease :
	case QEvent::MouseMove :
	case QEvent::Wheel :
	{
		QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
		// \todo This does not capture the mouse events generated by pen
		// Doing this at parent level will disrupt touch to m ouse conversions to other widgets too
		if ( (mouseEvent != NULL ) &&
			 (mouseEvent->source() != Qt::MouseEventSource::MouseEventNotSynthesized))
		{
			event->ignore();
			return true;
		}
	}
	default:
		break;
	}

	// Pause the physics when interacting with the components/drawing
	switch (eventType)
	{
	case QEvent::TouchBegin :
	case QEvent::TabletPress :
	case QEvent::MouseButtonPress :
	{
		_page->getPhysicsManager()->quickPause(true);
		break;
	}
	case QEvent::TouchEnd :
	case QEvent::TouchCancel :
	case QEvent::MouseButtonRelease :
	case QEvent::TabletRelease :
	{
		_page->getPhysicsManager()->quickPause(false);
		_page->onSimulationStepStart();	// Update changes to physics before next start
		break;
	}
	default:
		break;
	}

	// filter all events from controller
	if (_tapOverrideEnabled)
	{
		touchholdController->handleTapAndHold(event);	// Do not cancel events
		return false;	// Send events to scene but not to controller.
	}

	//QString msg = getEventname(event);
	//if (!msg.isEmpty()) QLOG_INFO() <<"Viewport event" << msg;

	switch(event->type())
	{
	case QEvent::TouchBegin :
	case QEvent::TouchUpdate :
	case QEvent::TouchEnd :
	case QEvent::TouchCancel :
	{
		if (_isStylusNearby) return true;	// Ignore all touch events when pen is nearby
		// Check if we are handling the events here
		bool retval =  onTouchEventFromView(static_cast<QTouchEvent*>(event));
		return retval;
	}
	case QEvent::MouseButtonDblClick :
	{
		QMouseEvent* mouseEvent = dynamic_cast<QMouseEvent*>(event);
		// Filter out the mouse double click generated by the stylus since it
		// it creates issues. Use only hand to touch
		if ( (mouseEvent != NULL ) &&
			 (mouseEvent->source() == Qt::MouseEventSource::MouseEventNotSynthesized))
		{
			event->ignore();
			return true;
		}
		return false;
	}
	case QEvent::MouseButtonPress :
	case QEvent::MouseButtonRelease :
	case QEvent::MouseMove :
	{
		if (!_isStylusNearby)
		{
			onMouseEventFromView(static_cast<QMouseEvent*>(event), _view);
		}
		break;
	}
	case QEvent::TabletPress :
	case QEvent::TabletMove :
	case QEvent::TabletRelease :
	{
		onTabletEventFromView(static_cast<QTabletEvent*>(event), _view);
		break;	// Let's try sending these to scene
	}
	case QEvent::Gesture :
	{
		QGestureEvent* gestureEvent = static_cast<QGestureEvent*>(event);
		onGestureEventFromView(gestureEvent);
		break;	// Let the scene handle the gesture as well.
	}
	default:
		break;
	}
	return false;
}

QList<Component*> PlayGoController::getSelectableComponentsByPhysics(QPointF scenePos)
{
	QList<QGraphicsItem*> selectedItems = _scene->items(scenePos, Qt::IntersectsItemBoundingRect);
	QList<Component*> itemsToUse;
	if (selectedItems.size() == 0) return itemsToUse;
	foreach (QGraphicsItem* graphicsitem, selectedItems)
	{
		if (graphicsitem->parentItem() == NULL) continue;
		if (graphicsitem->parentItem()->type() != Component::Type) continue;
		if (graphicsitem->type() == Pixmap::Type
				|| graphicsitem->type() == Polygon2D::Type  ) // Replace to accomodate other widgets
		{
			if (graphicsitem->contains(graphicsitem->mapFromScene(scenePos)))
			{
				Component* c = qgraphicsitem_cast<Component*>(graphicsitem->parentItem());
				itemsToUse.push_back(c);
			}
		}
	}
	return itemsToUse;
}

void PlayGoController::sketchAction(QTabletEvent *event)
{
	float pressure = 0.75f; //event->pressure();
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

void PlayGoController::sketchAction(QMouseEvent *event)
{
	float pressure = 0.75f; // In case of mouse, set it to 75% of max value
	QPointF sceneMappedPos = _view->mapToScene(event->pos());
	int time = 0;

	switch (event->type())
	{
		case QEvent::MouseButtonPress :
		{
			brushPress(sceneMappedPos, pressure, time);
			break;
		}
		case QEvent::MouseMove :
		{
			brushMove(sceneMappedPos, pressure, time);
			break;
		}
		case QEvent::MouseButtonRelease :
		{
			brushRelease(sceneMappedPos, pressure, time);
			break;
		}
	}
}

void PlayGoController::shapeAction(QTabletEvent *event)
{
	QPointF scenePos = _view->mapToScene(event->pos());
	switch (event->type())
	{
	case QEvent::TabletPress :
		shapePress(scenePos);
		break;
	case QEvent::TabletMove :
		shapeMove(scenePos);
		break;
	case QEvent::TabletRelease :
		shapeRelease(scenePos);
		break;
	}
}

void PlayGoController::shapeAction(QMouseEvent *event)
{
	QPointF scenePos = _view->mapToScene(event->pos());

	switch (event->type())
	{
		case QEvent::MouseButtonPress :
		{
			shapePress(scenePos);
			break;
		}
		case QEvent::MouseMove :
		{
			shapeMove(scenePos);
			break;
		}
		case QEvent::MouseButtonRelease :
		{
			shapeRelease(scenePos);
			break;
		}
	}
}

// \todo what is the eraser strength varies based on pressure value?
void PlayGoController::eraseAction(QTabletEvent *event)
{
	QPointF pos = _view->mapToScene(event->pos());

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

void PlayGoController::eraseAction(QMouseEvent *event)
{
	QPointF pos = _view->mapToScene(event->pos());

	switch (event->type())
	{
		case QEvent::MouseButtonPress :
		{
			eraserPress(pos);
			break;
		}
		case QEvent::MouseMove :
		{
			eraserMove(pos);
			break;
		}
		case QEvent::MouseButtonRelease :
		{
			eraserRelease(pos);
			break;
		}
	}
}

void PlayGoController::selectAction(QTabletEvent *event)
{
	QPointF pos = _view->mapToScene(event->pos());

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

void PlayGoController::selectAction(QMouseEvent *event)
{
	QPointF pos = _view->mapToScene(event->pos());

	switch (event->type())
	{
		case QEvent::MouseButtonPress :
		{
			lassoPress(pos);
			break;
		}
		case QEvent::MouseMove :
		{
			lassoMove(pos);
			break;
		}
		case QEvent::MouseButtonRelease :
		{
			lassoRelease(pos);
			break;
		}
	}
}

void PlayGoController::connectAction(QTabletEvent *event)
{
	QPointF sceneMappedPos = _view->mapToScene(event->pos());
	switch (event->type())
	{
	case QEvent::TabletPress :
		connectPress(sceneMappedPos);
		break;
	case QEvent::TabletMove :
		connectMove(sceneMappedPos);
		break;
	case QEvent::TabletRelease :
		connectRelease(sceneMappedPos);
		break;
	}
}

void PlayGoController::connectAction(QMouseEvent *event)
{
	QPointF sceneMappedPos = _view->mapToScene(event->pos());

	switch (event->type())
	{
		case QEvent::MouseButtonPress :
		{
			connectPress(sceneMappedPos);
			break;
		}
		case QEvent::MouseMove :
		{
			connectMove(sceneMappedPos);
			break;
		}
		case QEvent::MouseButtonRelease :
		{
			connectRelease(sceneMappedPos);
			break;
		}
	}
}

void PlayGoController::searchAction()
{
	// We are searching if at least one of the strokes is highlighted
	if (!_itemHighlighted) return;
	// Do not use lasso polygon to find the selected strokes because selection can be done in
	// multiple iterations
	QImage inputImage =
			//				_scene->getSelectionImage(_lassoPolygon);
			_page->getSelectionImageFromHighlight();
	auto selectedStrokes = _page->getHighlightedStrokes();

	if (selectedStrokes.size() == 0 || inputImage.isNull()) return;

	auto results =	_page->getSearchManager()->search(inputImage, 20);

	if (results.size() == 0) return;

	// Add overlay on the viewable scene
	QRect portRect = _view->viewport()->rect();
	QPolygonF scenePolygon= _view->mapToScene(portRect);

	QGraphicsPolygonItem* rectItem = new QGraphicsPolygonItem;
	QColor col = QColor(200,150,150,10);
	QBrush brush = QBrush(col);
	rectItem->setBrush(brush);
	rectItem->setPolygon(scenePolygon);

	QRect boundingRectOfPolygon = scenePolygon.boundingRect().toRect();	// bounding rect should be close to display area
	Q_UNUSED(boundingRectOfPolygon)

	int screen_width = portRect.width();
	int screen_height = portRect.height(); Q_UNUSED(screen_height)
	int screen_startx = portRect.left();
	int screen_starty = portRect.top();

	int each_image_width = 150;	// px
	int image_margin = 20;	// px
	int num_images = screen_width / (each_image_width + image_margin);

	//QLOG_INFO() << num_images	<< each_image_width << "(" << screen_width << "x" << screen_height << ")";

	searchItemGroup = new QGraphicsItemGroup();
	searchItemGroup->setFlag(QGraphicsItem::ItemIgnoresTransformations);
	_scene->addItem(searchItemGroup);
	searchItemGroup->setZValue(Z_UIVIEW);
	searchItemGroup->addToGroup(rectItem);

	int i=0; int j=0;
	{
		// Position the close button on the center
		QPoint screenPos = QPoint(screen_startx+ i * (each_image_width+image_margin) + each_image_width/2,
								  screen_starty+ j * (each_image_width+image_margin) + each_image_width/2);
		SelectableActions* selectable = new SelectableActions(hideSearchResultAction, nullptr);

		selectable->setPos(screenPos);
		selectable->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		searchItemGroup->addToGroup(selectable);
		++i;
	}

	for(SearchResult* searchResult : results)
	{
		QPoint screenPos = QPoint(screen_startx+ i * (each_image_width+image_margin),
								  screen_starty+ j * (each_image_width+image_margin));
		cdSearchGraphicsItem *resultItem = new cdSearchGraphicsItem(searchResult, each_image_width);
		resultItem->setPos(_view->mapToScene(screenPos));
		searchItemGroup->addToGroup(resultItem);

		connect(resultItem, SIGNAL(signalSearchItemSelected(cdSearchGraphicsItem*)),
				this, SLOT(onSearchItemSelect(cdSearchGraphicsItem*)));

		if (++i == num_images){ i=0; j++; }
	}
}

unsigned int PlayGoController::getPhysicsMask() const
{
	return _physicsMask;
}

void PlayGoController::setPhysicsMask(unsigned int newMask)
{
	if (newMask == _physicsMask) return;	// Do nothing

	PhysicsManager *physicsmanager = _page->getPhysicsManager();

	if (newMask == 0)
	{
		physicsmanager->pause();
		_physicsMask = newMask;
		return;
	}
	if (newMask & noGravity)	// Default: Gravity = ON
	{
//		physicsmanager->disableGravity();
	} else
	{
//		physicsmanager->enableGravity();
	}

	if (newMask & noCollision)	// Default: Collision = ON
	{

	} else
	{

	}

	if (newMask & noMotor) // Default: Motor = Enabled
	_physicsMask = newMask;
}

bool PlayGoController::isTapOverrideEnabled() const
{
	return _tapOverrideEnabled;
}

void PlayGoController::setTapOverride(bool value)
{
	if (_tapOverrideEnabled == value) return;

	_tapOverrideEnabled = value;
	auto components = _page->getComponents();
	bool touchEnable = (_tapOverrideEnabled ? false : true);
	for(Component* component : components)
		component->setAcceptTouchEvents(touchEnable);
}

void PlayGoController::overrideOnTapAndHold(QTapAndHoldGesture *gesture)
{
	QPoint pos = gesture->position().toPoint();
	QPoint viewportPos = _viewport->mapFromGlobal(pos);
	QPointF scenePos = _view->mapToScene(viewportPos.x(), viewportPos.y());

	QLOG_INFO() << pos << viewportPos << scenePos;

	// Erase whatever was done till now
	switch (_activeMode)
	{
	case UI::Sketch :
	{
		if (_isDrawingStroke && _currentStroke != NULL)
		{
			delete _currentStroke; _currentStroke = NULL;
			_isDrawingStroke = false;
		}
		break;
	}
	case UI::Shapes :
		if (_isDrawingPolygon && _currentPolygon != NULL)
		{
			if (_currentPolygon->parentItem())
			{
				QGraphicsItem *parentItem = _currentPolygon->parentItem();
				if (parentItem->type() == Component::Type)
				{		_page->destroyComponent
							(qgraphicsitem_cast<Component*>(parentItem));
				}
				else
				{
					delete _currentPolygon;
				}
			} else
			{
				delete _currentPolygon;
			}
			_isDrawingPolygon = false;
			_currentPolygon = 0;
		}
		break;
	case UI::Connect :
	{
		if (_activeConnectionMode == SliderJoint)
		{
			_sliderComponentA = 0;
			_sliderComponentB = 0;
			if (_sliderLineItem) delete _sliderLineItem;
			_sliderLineItem = 0;
		}
		break;
	}
	case UI::None :
	{
		pauseSimulation();
		break;
	}
	}

	// Get underlying component
	QList<QGraphicsItem*> selectedItems = _page->scene()->items(scenePos,
																Qt::IntersectsItemBoundingRect,
																Qt::AscendingOrder, _view->transform());

	//look for joints first
	for (QList<QGraphicsItem*>::const_iterator it = selectedItems.constBegin();
		 it != selectedItems.constEnd(); ++it)
	{
		QGraphicsItem* graphicsitem = (*it);
		if (graphicsitem->type() == cdJoint::Type)
		{
			cdJoint* physicsJoint = qgraphicsitem_cast<cdJoint*>(graphicsitem);
			touchholdController->enableOverlay(physicsJoint, scenePos);
			//_view->centerOn(scenePos);
			return;
		}
	}
	// create component radial menu first
	Component * selectedComponent = 0;
	for (QList<QGraphicsItem*>::const_iterator it = selectedItems.constBegin();
		 it != selectedItems.constEnd(); ++it)
	{
		QGraphicsItem* graphicsitem = (*it);
		if (graphicsitem->type() == Component::Type)
		{
			selectedComponent = qgraphicsitem_cast<Component*>(graphicsitem);
			break;
		}
		if ((graphicsitem->type() == Polygon2D::Type  || graphicsitem->type() == Pixmap::Type)
				&& graphicsitem->parentItem() != NULL)
		{
			selectedComponent = qgraphicsitem_cast<Component*>(graphicsitem->parentItem());
			break;
		}
	}
	if (selectedComponent)
	{
		touchholdController->enableOverlay(selectedComponent, scenePos);

		// _view->centerOn(scenePos);	// centering has odd effect on eyes
	}
}

void PlayGoController::stylusEnter()
{
	_isStylusNearby = true;
}

void PlayGoController::stylusLeave()
{
	_isStylusNearby = false;
}

void PlayGoController::setAcceptComponentTouch(bool value)
{
	QList<Component*> components = _page->getComponents();
	for (QList<Component*>::const_iterator iter = components.constBegin();
		 iter != components.constEnd(); ++iter)
	{
		Component *component = (*iter);
		component->setAcceptTouchEvents(value);
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
	QList<Stroke*> selectedStrokes = _page->getHighlightedStrokes();
	if (selectedStrokes.size() == 0)
	{
		// If selected strokes are empty.. this will create
		QMessageBox::about
				(_toplevelWindow, "Select search",
				 "No stroke selected for replacing");
		return;
	}

	QList<QGraphicsItem*> graphicsitems;
	for (int i=0; i < selectedStrokes.size(); i++)
		graphicsitems.push_back(selectedStrokes[i]);

	QRectF itemRect = _page->getBoundingBox(graphicsitems);
	qDeleteAll(selectedStrokes);
	selectedStrokes.clear();
	graphicsitems.clear();

	// Always set component position before adding objects to it
	// Delete the strokes once done with adding
	QPixmap _pixmap = QPixmap();
	_pixmap.load(result->resultFilePath);
	Pixmap *pixmap = new Pixmap(_pixmap, result->resultFilePath);

	Component * newComponent = _page->createComponent();
	newComponent->addToComponent(pixmap);   // Physics shape is initialized here
	//			pixmap->setTransform(QTransform());
	//			pixmap->setPos(0,0);
	//newComponent->setTransform(QTransform().translate(itemRect.left(), itemRect.top()));
	newComponent->moveBy(itemRect.left(), itemRect.top());

	QSize pixmapSize = _pixmap.size();//   ->boundingRect();

	float scale = itemRect.width()/pixmapSize.width() < itemRect.height()/pixmapSize.height() ?
				itemRect.width()/pixmapSize.width() : itemRect.height()/pixmapSize.height();
	newComponent->setScale(scale);	// scaling ruins the fixtures. needs regeneration
	newComponent->internalTransformChanged();
	newComponent->requiresRegeneration = true;
}

void PlayGoController::onSearchItemSelect(cdSearchGraphicsItem *result)

{
	onSearchItemSelect(&result->getResult());
	closeSearchResultDisplay();
}

void PlayGoController::closeSearchResultDisplay()
{
	if (searchItemGroup)
	{
		delete searchItemGroup;
		searchItemGroup = nullptr;
	}
}

void PlayGoController::onTabletEventFromView(QTabletEvent *event,
											 QGraphicsView *view)
{
	Q_UNUSED(view);
	if (event->pointerType() == QTabletEvent::Pen)
	{
		switch (_activeMode)
		{
		case UI::Sketch :
			sketchAction(event);
			break;
		case UI::Shapes :
			shapeAction(event);
			break;
		case UI::Erase :
			eraseAction(event);
			break;
		case UI::Select :
			selectAction(event);
			break;
		case UI::Connect :
			connectAction(event);
			break;
		default:
			break;
		}
	} else if (event->pointerType() == QTabletEvent::Eraser)
	{
		eraseAction(event);	// Let the eraser decide what action to take based on mode.
	}
}

void PlayGoController::onMouseEventFromView(QMouseEvent *event, QGraphicsView *view)
{
	Q_UNUSED(view)
	switch (_activeMode)
	{
	case UI::Sketch :
		sketchAction(event);
		break;
	case UI::Shapes :
		shapeAction(event);
		break;
	case UI::Erase :
		eraseAction(event);
		break;
	case UI::Select :
		selectAction(event);
		break;
	case UI::Connect :
		connectAction(event);
		break;
	}
}

namespace
{
	bool eventAcceptedByJoint = false;
	cdJoint *touchEventOwner = NULL;
	QPointF initialPos;
}
bool PlayGoController::onTouchEventFromView(QTouchEvent *event)
{
	QTransform inverted = _view->transform().inverted();
	if (event->touchPoints().count() == 1)
	{
		if (_activeMode == UI::Connect)
		{
			if (event->type() == QEvent::TouchBegin)
			{
				eventAcceptedByJoint = false;
				touchEventOwner = 0;
				const QTouchEvent::TouchPoint &tp = event->touchPoints().first();
				QPointF scenePos = _view->mapToScene(tp.pos().toPoint());//tp.scenePos();
				QLOG_INFO() << scenePos << tp.scenePos();

				auto selectedItems = _scene->items(scenePos, Qt::IntersectsItemBoundingRect,
																	Qt::DescendingOrder, _view->transform());
				for(QGraphicsItem* graphicsitem : selectedItems)
				{
					if (graphicsitem->type() == cdJoint::Type && graphicsitem->parentItem())
					{
						if (graphicsitem->contains(graphicsitem->mapFromScene(scenePos)))
						{
							auto physicsJoint = qgraphicsitem_cast<cdJoint*>(graphicsitem);
							eventAcceptedByJoint = true;
							touchEventOwner = physicsJoint;
							initialPos = physicsJoint->pos();
							event->accept();
							return true;
						}
					}
				}
			}

			if (event->type() == QEvent::TouchUpdate && eventAcceptedByJoint)
			{
				QGraphicsItem* parentItem = touchEventOwner->parentItem();
				const QTouchEvent::TouchPoint &tp = event->touchPoints().first();
				QPointF difference = parentItem->mapFromScene(tp.scenePos()) - parentItem->mapFromScene(tp.lastScenePos());
				touchEventOwner->moveBy(difference.x(), difference.y());
				return true;
			}
			if (event->type() == QEvent::TouchEnd && eventAcceptedByJoint)
			{
				_page->getPhysicsManager()->moveJoint(touchEventOwner, touchEventOwner->scenePos());

				eventAcceptedByJoint = false;
				touchEventOwner = 0;
				return true;
			}
			if (event->type() == QEvent::TouchCancel && eventAcceptedByJoint)
			{
				touchEventOwner->setPos(initialPos);
				eventAcceptedByJoint = false;
				touchEventOwner = 0;
			}
			return false;
		}
	}
	else if (event->touchPoints().count() == 2)
	{
		return false;
	}
	else if (event->touchPoints().count() == 3)
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
			previousCenter += touch.lastPos();
			currentCenter += touch.pos();
		}

		previousCenter = previousCenter /3;
		currentCenter = currentCenter /3;

		QPointF difference = inverted.map(currentCenter) - inverted.map(previousCenter);
		//			_view->translate(difference.x(), difference.y());
		QTransform t;
		t.translate(difference.x(), difference.y());
		{
			//scaling
			QPointF a0 = tp1.lastPos(); QPointF a1 = tp1.pos();
			QPointF b0 = tp2.lastPos(); QPointF b1 = tp2.pos();
			QPointF c0 = tp3.lastPos(); QPointF c1 = tp3.pos();

			float d0 = diameterOfCircumcircle(euclideanDistance(&a0,&b0),
											  euclideanDistance(&b0,&c0),
											  euclideanDistance(&c0,&a0));

			float d1 = diameterOfCircumcircle(euclideanDistance(&a1,&b1),
											  euclideanDistance(&b1,&c1),
											  euclideanDistance(&c1,&a1));

			if ((qFuzzyCompare(d0,0) == false && qFuzzyCompare(d1,0)) == false)
			{
				t.scale(d1/d0,d1/d0);
				//_view->scale(d1/d0,d1/d0);
			}
		}
		QPointF currentSceneCenter = _view->mapToScene(currentCenter.toPoint());

		_view->setTransform(t, true);

		QPointF updatedViewCenter = _view->mapFromScene(currentSceneCenter);
		difference = currentCenter - updatedViewCenter;
		t = QTransform();
		t.translate(difference.x(), difference.y());
		_view->setTransform(t, true);

		event->accept();
		return true;
	}
	return false;
}

bool PlayGoController::onTouchEventFromScene(QTouchEvent *event)
{
	// Here the event is not accepted and therefore we are free to do whatever we wish
	// NOTE - This might create issues when we are displaying scene in a view != sketchview
	if (event->touchPoints().count() == 1)
	{
		// pan

	} else if (event->touchPoints().count() == 2)
	{

	}

	return false;
}

void PlayGoController::onGestureEventFromView(QGestureEvent *event)
{
	if (QTapAndHoldGesture *tap_and_hold =
			static_cast<QTapAndHoldGesture*>(event->gesture(Qt::TapAndHoldGesture)))
	{
		if (tap_and_hold->state() == Qt::GestureStarted)
		{
			tap_and_hold->setGestureCancelPolicy(QGesture::CancelAllInContext);
			event->accept();
		}
		if (tap_and_hold->state() == Qt::GestureFinished)
		{
			overrideOnTapAndHold(tap_and_hold);
		}
	}

	if (QTapGesture *tap = static_cast<QTapGesture*>(event->gesture(Qt::TapGesture)))
	{
		switch (tap->state())
		{
		case Qt::GestureStarted :
		{
			break;
		}
		case Qt::GestureUpdated :
		{
			break;
		}
		case Qt::GestureFinished :
		{
			break;
		}
		case Qt::GestureCanceled :
		{
			break;
		}
		}
	}

	QString msg =  "Gesture received>>";
	if (QTapGesture *tap = static_cast<QTapGesture*>(event->gesture(Qt::TapGesture)))
	{
		msg = msg +  "Tap Gesture>>";
		if (tap->state() == Qt::GestureStarted)
			msg += "Started>>";
		if (tap->state() == Qt::GestureUpdated)
			msg += "Updated>>";
		if (tap->state() == Qt::GestureFinished)
			msg += "Finished>>";
		if (tap->state() == Qt::GestureCanceled)
			msg += "Canceled>>";
	}
	if (QTapAndHoldGesture *tap_and_hold =
			static_cast<QTapAndHoldGesture*>(event->gesture(Qt::TapAndHoldGesture)))
	{
		msg = msg + "Tap and Hold Gesture>>";
		if (tap_and_hold->state() == Qt::GestureStarted)
		{
			msg += "Started>>";
		}
		if (tap_and_hold->state() == Qt::GestureUpdated)
			msg += "Updated>>";
		if (tap_and_hold->state() == Qt::GestureFinished)
		{
			msg += "Finished>>";
		}
		if (tap_and_hold->state() == Qt::GestureCanceled)
			msg += "Canceled>>";
	}
	if (QPanGesture *pan = static_cast<QPanGesture*>(event->gesture(Qt::PanGesture)))
	{
		msg = msg + "Pan Gesture>>";
		if (pan->state() == Qt::GestureStarted)
			msg += "Started>>";
		if (pan->state() == Qt::GestureUpdated)
			msg += "Updated>>";
		if (pan->state() == Qt::GestureFinished)
			msg += "Finished>>";
		if (pan->state() == Qt::GestureCanceled)
			msg += "Canceled>>";
	}
	if (QSwipeGesture *swipe = static_cast<QSwipeGesture *>(event->gesture(Qt::PanGesture)))
	{
		msg = msg + "Swipe Gesture>>";
		if (swipe->state() == Qt::GestureStarted)
			msg += "Started>>";
		if (swipe->state() == Qt::GestureUpdated)
			msg += "Updated>>";
		if (swipe->state() == Qt::GestureFinished)
			msg += "Finished>>";
		if (swipe->state() == Qt::GestureCanceled)
			msg += "Canceled>>";
	}
	QLOG_INFO() << msg;
}

void PlayGoController::onPhysicsMaskUpdate()
{

}

void PlayGoController::connectionModeReset()
{
	forceLine = NULL;

	_sliderComponentA = NULL;
	_sliderComponentB = NULL;
	_sliderStartPos = QPointF();
	_sliderEndPos = QPointF();
	_sliderLineItem = NULL;
}

void PlayGoController::setToDraw()
{
	setMode(UI::Sketch);
}

void PlayGoController::setToShape()
{
	setMode(UI::Shapes);	// allow drawing of polygons
}

void PlayGoController::setToConnectorMode()
{
	setMode(UI::Connect);
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

void PlayGoController::setModeScribble()
{
	_activeConnectionMode = GestureSketch;
}

void PlayGoController::setModeLockItem()
{
	connectionModeReset();
	_activeConnectionMode  = StaticJoint;
}

void PlayGoController::setModeHingeJoint()
{
	//		QMessageBox::about(NULL, "MSG", "Set to hinge mode");
	connectionModeReset();
	_activeConnectionMode = HingeJoint;
}

void PlayGoController::setModeSliderJoint()
{
	connectionModeReset();
	_activeConnectionMode = SliderJoint;
}

void PlayGoController::setModeSpringJoint()
{
	//		QMessageBox::about(NULL, "MSG", "Set to spring mode");
	connectionModeReset();
	_activeConnectionMode = SpringJoint;
}

void PlayGoController::setModeForce()
{
	//		QMessageBox::about(NULL, "MSG", "Set to force mode");
	connectionModeReset();
	_activeConnectionMode = ApplyForce;
}

void PlayGoController::setMode(UI::MODE newMode)
{
	if (_tapOverrideEnabled)
		touchholdController->slotCloseOverlay();
	_page->getPhysicsManager()->setEnableDebugView(false);
	onModeChange(_activeMode, newMode);
	if (_activeMode == newMode)
	{
		return;
	}
	_lastActiveMode		= _activeMode;
	_activeMode			= newMode;
}

void PlayGoController::clearCurrentScene()
{
	_page->deleteAll();
	_currentStroke = NULL;
}

void PlayGoController::drawMenusOnView(QPainter * painter, const QRectF & rect)
{
	Q_UNUSED(painter)
	Q_UNUSED(rect)
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
	if (_pixmap.width() > 500 || _pixmap.height() > 500)
	{
		_pixmap = _pixmap.scaled(QSize(500,500), Qt::KeepAspectRatio);
	}
	Pixmap* pixmap = new Pixmap(_pixmap, imagePath);
	Component* component = _page->createComponent();
	component->addToComponent(pixmap);
}

void PlayGoController::loadImage(QString imagePath, QObject* obj, QDropEvent* event)
{
	Q_UNUSED(obj)	// TODO - Use this to check where the image came from
	QPixmap _pixmap = QPixmap();
	_pixmap.load(imagePath);
	// TODO - This might actually be needed but keep it to reduce the memory footprint
	if (_pixmap.width() > 500 || _pixmap.height() > 500)
	{
		_pixmap = _pixmap.scaled(QSize(500,500), Qt::KeepAspectRatio);
	}
	Pixmap* pixmap = new Pixmap(_pixmap, imagePath);
	Component* component = _page->createComponent();
	component->addToComponent(pixmap);

	QPointF scenePos = _view->mapToScene(event->pos());
	scenePos = scenePos - QPointF(_pixmap.width()/2.0f, _pixmap.height()/2.0f);
	component->moveBy(scenePos.x(), scenePos.y());

//	QGraphicsOpacityEffect *opacityEffect = new QGraphicsOpacityEffect;
//	opacityEffect->setOpacity(0.85);
//	component->setGraphicsEffect(opacityEffect);

//	QGraphicsBlurEffect *blurEffect = new QGraphicsBlurEffect;
//	blurEffect->setBlurRadius(4.0f);
//	component->setGraphicsEffect(blurEffect);

//	QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect;
//	shadowEffect->setBlurRadius(4.0f);
//	shadowEffect->setColor(QColor(80,80,80,200));
//	shadowEffect->setOffset(4);
//	component->setGraphicsEffect(shadowEffect);

	QGraphicsColorizeEffect *colorize = new QGraphicsColorizeEffect;
	colorize->setColor(Qt::red);
	colorize->setStrength(0.1);
	component->setGraphicsEffect(colorize);

	QPropertyAnimation *animation = new QPropertyAnimation(colorize, "strength");
	animation->setDuration(2000);
	animation->setStartValue(0.1f);
	animation->setEndValue(1.0f);
	animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void PlayGoController::startSimulation()
{
	if (_page)
	{
		if (_page->scene() && _page->getPhysicsManager()->debugView)
			_page->getPhysicsManager()->debugView->scene = _page->scene();
		setMode(UI::None);
		_page->getPhysicsManager()->start(20);
		_page->getPhysicsManager()->setEnableMotor(true);
	}
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
