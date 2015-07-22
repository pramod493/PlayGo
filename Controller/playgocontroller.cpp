#include "PlayGoController.h"
#include <QLabel>
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
#include "cdiboxdebugdraw.h"

namespace CDI
{

PlayGoController::PlayGoController(SketchView *view, CDIWindow *parent)
    :QObject(parent)
{

	if (view == NULL || parent == NULL)
    {
        QLOG_FATAL() << "Invalid scene and view reference.";
        QMessageBox::about(NULL, "Error message",
                           "Cannot initialize controller. Invalid arguments in the constructor");
    }

    // Hide display of the model tree...
    //		tree = new ModelViewTreeWidget(parent->playgo);
    //		tree->show();
    tree = NULL;

    _toplevelWindow = parent;
	_scene = static_cast<SketchScene*>(view->getPage()->scene());
    _view = view;
    _viewport = view->viewport();
	_page = view->getPage();

    // Do not install event filter on QGraphicsView but on viewport
    // Installing event on QGraphicsView seems to filter out touch
    _view->viewport()->installEventFilter(this);

	_view->setTransformationAnchor(QGraphicsView::NoAnchor);
	_view->setOptimizationFlag(QGraphicsView::DontSavePainterState);
	_view->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

    connect(_view, SIGNAL(viewDrawforeground(QPainter*,const QRectF&)),
            this, SLOT(drawMenusOnView(QPainter*,const QRectF&)));

    connect(_view, SIGNAL(viewImageDrop(QString, QObject*, QDropEvent*)),
            this, SLOT(loadImage(QString, QObject*, QDropEvent*)));

    initController();

    searchView = _toplevelWindow->searchView;
    if (searchView != NULL)
    {
        connect(searchView, SIGNAL(signalOnSearchResultSelect(SearchResult*)),
                this, SLOT(onSearchItemSelect(SearchResult*)));
    }
}

PlayGoController::~PlayGoController()
{
	if (tree != NULL) delete tree;
}

void PlayGoController::initController()
{
	// Set the deafult value of other settings
	_mouseModeEnabled = true;

    _activeMode = UI::Sketch;
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

    _isDrawingNow = false;
    _currentStroke = NULL;
    _currentPolygon = NULL;

    _lasso = new QGraphicsPolygonItem();
    _lasso->setPen(_lassoPen);
    _lasso->setBrush(_lassoBrush);
    _scene->addItem(_lasso);
    _isLassoDisplayed = false;

    _itemHighlighted = false;

    ///////////////////////////////////////////////////////
    // Connection option settings
    createConnectionsToolbar();
    hideConnectionsToolbar();
    _currentMode = HingeJoint;
    _tmpStrokes = QList<PenStroke*>();
    _currentConnectStroke = NULL;
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
    //		_scene->update(_currentStroke->mapRectToScene(_currentStroke->boundingRect()));
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

void PlayGoController::shapePress(QPointF pos)
{
    // create new component
    Component *newComponent = _page->createComponent();
    _currentPolygon = new Polygon2D();
    newComponent->addToComponent(_currentPolygon);
    //		_currentPolygon->setPos(0,0);
    newComponent->moveBy(pos.x(),pos.y());

    _currentPolygon->push_point(pos);

#ifdef CDI_DEBUG_DRAW_SHAPE
    QLOG_INFO() << "Component" << newComponent->transform();
    QLOG_INFO() << "Component-Scene" << newComponent->sceneTransform();
    QLOG_INFO() << "Polygon2D" << _currentPolygon->transform();
    QLOG_INFO() << "Polygon2D-Scene" << _currentPolygon->sceneTransform();
#endif //CDI_DEBUG_DRAW_SHAPE

    _fillPen = _defaultPen;
	QColor tmpColor = _fillPen.color();
	tmpColor.setAlpha(150);
	_fillBrush.setColor(tmpColor);
    _currentPolygon->setPen(_defaultPen);
    _currentPolygon->setBrush(_fillBrush);
}

void PlayGoController::shapeMove(QPointF pos)
{
    if (_currentPolygon)
        _currentPolygon->push_point(pos);
}

void PlayGoController::shapeRelease(QPointF pos)
{
    if (_currentPolygon)
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
            }
        }
        _currentPolygon = NULL;
    }
}

void PlayGoController::eraserPress(Point2DPT pos)
{
    eraserMove(pos);
}

void PlayGoController::eraserMove(Point2DPT pos)
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

	QList<Stroke*> selectedStrokes = _page->getSelectedStrokes(_lassoPolygon, 1.0f);
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

void PlayGoController::connectPress(QPointF scenePos)
{
    switch (_currentMode)
    {
    case GestureSketch :
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
    case HingeJoint :
    {
        QList<Component*> itemsToUse = getSelectableComponentsByPhysics(scenePos);

        if (itemsToUse.size() == 1)
        {
            //				float physicsScale = getPhysicsScale();
            //				// Pin against world
            //				Component *c1 = itemsToUse.first();

            //				b2RevoluteJointDef revolutJointDef;
            //				b2Body* fixedBody = 0;
            //				{
            //					b2BodyDef def;
            //					def.type = b2_staticBody;
            //					def.position = b2Vec2(0,0);	//b2Vec2(scenePos.x()/physicsScale, scenePos.y()/physicsScale);
            //					def.angle = 0;
            //					fixedBody = _page->getPhysicsManager()->_b2World->CreateBody(&def);
            //				}


            //				revolutJointDef.bodyA = fixedBody;
            //				revolutJointDef.bodyB = c1->physicsBody();
            //				revolutJointDef.collideConnected = false;

            //				QPointF localPos1 = scenePos;
            //				QPointF localPos2 = c1->mapFromScene(scenePos);

            //				revolutJointDef.localAnchorA.Set(localPos1.x()/physicsScale, localPos1.y()/physicsScale);
            //				revolutJointDef.localAnchorB.Set(localPos2.x()/physicsScale, localPos2.y()/physicsScale);

            //				revolutJointDef.enableLimit = false;

            //				revolutJointDef.enableMotor = true;
            //				revolutJointDef.maxMotorTorque = 200;
            //				revolutJointDef.motorSpeed = 100*3.14f;	// 10 rpm

            //				_page->getPhysicsManager()->_b2World->CreateJoint(&revolutJointDef);
        }
        if (itemsToUse.size() >= 2)
        {	// Ignore all other components under selection
            Component *c1 = itemsToUse[0];
            Component *c2 = itemsToUse[1];

            if (c1->id() == c2->id())
            {
                if (itemsToUse.size() > 2)
                {
                    c2 = itemsToUse[2];
                }
            }

            /*
                b2RevoluteJointDef revolutJointDef;

                revolutJointDef.bodyA = c1->physicsBody();
                revolutJointDef.bodyB = c2->physicsBody();
                revolutJointDef.collideConnected = false;

                QPointF localPos1 = c1->mapFromScene(scenePos) * c1->scale();
                QPointF localPos2 = c2->mapFromScene(scenePos) * c2->scale();
                float physicsScale = getPhysicsScale();
                revolutJointDef.localAnchorA.Set(localPos1.x()/physicsScale, localPos1.y()/physicsScale);
                revolutJointDef.localAnchorB.Set(localPos2.x()/physicsScale, localPos2.y()/physicsScale);

                revolutJointDef.enableLimit = false;*/

            bool b_enableMotor = false;
            float f_motorSpeed = 0;
            float f_motorTorque = 0;
            if (enableMotorCheckbox->isChecked())
            {
                b_enableMotor = true;
                bool ok;
                int speed = motorSpeed->text().toInt(&ok);
                if (ok) f_motorSpeed = speed;		//revolutJointDef.motorSpeed = speed * 2.0f *3.14f;
                int torque = motorTorque->text().toInt(&ok);
                if (ok)	 f_motorTorque = torque;	//revolutJointDef.maxMotorTorque = torque;
            }
            PhysicsJoint* physicsJoint = _page->getPhysicsManager()->createPinJoint(
                        c1, c2, scenePos,
                        b_enableMotor, false,
                        f_motorSpeed, f_motorTorque,
                        0, 0);
			JointGraphics* jointView = new JointGraphics(physicsJoint, c1);
			jointView->setPos(c1->mapFromScene(scenePos));

//            float radius = 15.0f;
//            QGraphicsEllipseItem* marker1 = new QGraphicsEllipseItem();
//            c1->addToComponent(marker1);
//            marker1->setPos(c1->mapFromScene(scenePos));
//            marker1->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
//            marker1->setRect(-radius, -radius, radius, radius);


//            QGraphicsEllipseItem* marker2 = new QGraphicsEllipseItem();
//            c2->addToComponent(marker2);
//            marker2->setPos(c2->mapFromScene(scenePos));
//            marker2->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
//            marker2->setRect(-radius, -radius, radius, radius);
        }
        break;
    }
    case SliderJoint :
    {
        if (_sliderComponentA && _sliderComponentB)
        {
            _sliderStartPos = scenePos;
            if (_sliderLineItem) delete _sliderLineItem;
            _sliderLineItem = new QGraphicsLineItem(QLineF(_sliderStartPos, _sliderStartPos));
            _scene->addItem(_sliderLineItem);
        }
        break;
    }
    case SpringJoint :
        break;
    case ApplyForce :
    {
        // TODO - Default to application of force
        QList<QGraphicsItem*> selections = _scene->items(scenePos,
                                                         Qt::IntersectsItemBoundingRect);
        if (selections.size() == 0) break;

        foreach(QGraphicsItem* graphicsitem, selections)
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
            forceLine = new ForceGraphicsItem(0,0,0,0);
            forceLine->setTransform(t);
            _scene->addItem(forceLine);
            component->addToComponent(forceLine);
            return;
        }
        break;
    }
    }
}

void PlayGoController::connectMove(QPointF scenePos)
{
    switch (_currentMode)
    {
    case GestureSketch :
    {
        if (_currentConnectStroke)
            _currentConnectStroke->push_point(scenePos);
        break;
    }
    case HingeJoint :
        break;
    case SliderJoint :
    {
        if (_sliderLineItem)
        {
            _sliderEndPos = scenePos;
            _sliderLineItem->setLine(QLineF(_sliderStartPos, _sliderEndPos));
        }
        break;
    }
    case SpringJoint :
        break;
    case ApplyForce :
    {
        if (forceLine)
        {
            QPointF tPos = forceLine->mapFromScene(scenePos);
            forceLine->setLine(0,0,tPos.x(),tPos.y());
        }
        break;
    }
    }
}

void PlayGoController::connectRelease(QPointF scenePos)
{
    switch (_currentMode)
    {
    case GestureSketch :
    {
        if (_currentConnectStroke)
        {
            _currentConnectStroke->push_point(scenePos);

            sketchRecognizer->addStroke(_currentConnectStroke->points);
            _tmpStrokes.push_back(_currentConnectStroke);
            sketchRecognizer->gbRecognize();

            _currentConnectStroke = NULL;
        }
        break;
    }
	case StaticJoint :
	{
		QList<Component*> components = getSelectableComponentsByPhysics(scenePos);
		if (components.size())
		{
			Component* component = components[0];
			if (component->isStatic())
				component->setStatic(false);
			else
				component->setStatic(true);
		}
		break;
	}
    case HingeJoint :
        break;
    case SliderJoint :
    {
        if (_sliderComponentA == NULL)
        {
            QList<Component*> selection = getSelectableComponentsByPhysics(scenePos);
            if (selection.size())
            {
                _sliderComponentA = selection[0];
                _sliderComponentA->setOpacity(1.0f);
            }
        } else if (_sliderComponentB == NULL)
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
            // Check for lines
            QLineF lineA = QLineF(_sliderComponentA->mapFromScene(_sliderStartPos),
                                  _sliderComponentA->mapFromScene(_sliderEndPos));
            QLineF lineB = QLineF(_sliderComponentB->mapFromScene(_sliderStartPos),
                                  _sliderComponentB->mapFromScene(_sliderEndPos));

            QVector2D vecA = QVector2D(lineA.p2() - lineA.p1());
            //QVector2D vecB = QVector2D(lineB.p2() - lineB.p1());
            QVector2D vectorGlobal = QVector2D(_sliderEndPos-_sliderStartPos);

            float physicsScale = getPhysicsScale();

            b2PrismaticJointDef sliderDef;
            sliderDef.bodyA = _sliderComponentA->physicsBody();
            sliderDef.bodyB = _sliderComponentB->physicsBody();
            sliderDef.collideConnected = false;

            sliderDef.localAxisA.Set(vecA.x(), vecA.y());
            sliderDef.localAxisA.Normalize();

            sliderDef.localAnchorA.Set(lineA.p1().x()/physicsScale, lineA.p1().y()/physicsScale);
            sliderDef.localAnchorB.Set(lineB.p1().x()/physicsScale, lineB.p1().y()/physicsScale);

            sliderDef.enableLimit = true;
            sliderDef.lowerTranslation = 0;
            sliderDef.upperTranslation = vectorGlobal.length()/physicsScale;

            if (enableMotorCheckbox->isChecked())
            {
                sliderDef.enableMotor = true;
                bool ok;
                int speed = motorSpeed->text().toInt(&ok);
                if (ok) sliderDef.motorSpeed = speed;
                int torque = motorTorque->text().toInt(&ok);
                if (ok)	sliderDef.maxMotorForce = torque;
            } else
            {
                sliderDef.enableMotor = false;
            }

            b2Joint* joint = NULL;	//_page->getPhysicsManager()->_b2World->CreateJoint(&sliderDef);
            b2PrismaticJoint* sliderJoint = static_cast<b2PrismaticJoint*>(joint);
            //                _sliderComponentA->physicsBody()->SetType(b2_kinematicBody);
            _sliderComponentA->addToComponent(_sliderLineItem);

            _sliderComponentA->setOpacity(0.5f);
            _sliderComponentB->setOpacity(0.5f);

            _sliderLineItem = 0;
            _sliderComponentA = 0;
            _sliderComponentB = 0;
        }
        break;
    }
    case SpringJoint :
        break;
    case ApplyForce :
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

                    qDebug() << "Force output" << forceorigin << forcevector;
                }
            }
        }
        delete forceLine;
        forceLine = NULL;
        break;
    }
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

    connectionOptionsToolbar = new QToolBar(tr("Connect mode toolbar"), _toplevelWindow);

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
    QValidator *speedvalidator = new QIntValidator(100,1000, motorSpeed);
    motorSpeed->setValidator(speedvalidator);
    QLabel *speedlabel = new QLabel(QString("Motor speed:"));
    connectionOptionsToolbar->addWidget(speedlabel);
    connectionOptionsToolbar->addWidget(motorSpeed);
    motorSpeed->setText(QString("100"));

    connectionOptionsToolbar->addSeparator();

    motorTorque = new QLineEdit();
    QValidator *torqueValidator = new QIntValidator(10,10000, motorTorque);
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
        _isDrawingNow = false;
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
            component->setOpacity(0.5f);
        showConnectionsToolbar();
        connectionModeReset();
    }
    return retval;
}

bool PlayGoController::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == _viewport)
    {
        switch(event->type())
        {
        case QEvent::TouchBegin :
        case QEvent::TouchUpdate :
        case QEvent::TouchEnd :
        case QEvent::TouchCancel :
        {
            bool retval =  onTouchEventFromView(static_cast<QTouchEvent*>(event));
			return retval;
            // Let the scene handle its own touch events in any way it sees fit
            break;
        }
        case QEvent::MouseButtonDblClick :
        case QEvent::MouseButtonPress :
        case QEvent::MouseButtonRelease :
        case QEvent::MouseMove :
        {
            if (_mouseModeEnabled) return false;
            // Ignores all  mouse events
			/*event->accept();
			return true;	// consumes event*/
            break;
        }
        case QEvent::TabletPress :
        case QEvent::TabletMove :
        case QEvent::TabletRelease :
        {
            onTabletEventFromView(static_cast<QTabletEvent*>(event), _view);
			/*event->accept();
			return true;	// don't consume event - */
            break;
        }
        case QEvent::Gesture :
        {
            QGestureEvent* gestureEvent = static_cast<QGestureEvent*>(event);
			onGestureEventFromView(gestureEvent);
//			event->accept();
//			return true;
            break;
        }
        }
    }
    //		qDebug() << "Some other event. Ignore";
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

void PlayGoController::searchAction()
{

    /*Enable WebView
         * {
            // test display of QWebView
            // https://www.google.com/search?hl=en&site=imghp&tbm=isch&q=gears&tbs=ic:trans
            QGraphicsWebView *webview = new QGraphicsWebView(NULL);
            QUrl url = QUrl(QString("https://www.google.com/search?hl=en&site=imghp&tbm=isch&q=gears&tbs=ic:trans"));
            webview->setUrl(url);
            webview->show();
            qDebug() << "Should show web view";
        }*/
    // We are searching if at least one of the strokes is highlighted
    if (!_itemHighlighted) return;
    // Do not use lasso polygon to find the selected strokes because selection can be done in
    // multiple iterations
    QImage inputImage =
            //				_scene->getSelectionImage(_lassoPolygon);
			_page->getSelectionImageFromHighlight();
    QList<Stroke*> selectedStrokes =
			_page->getHighlightedStrokes();

    if (selectedStrokes.size() == 0) return;

    QList<SearchResult*> searchResults =
			_page->getSearchManager()->search(inputImage, 20);

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

    /*for (int i=0; i < selectedStrokes.size(); i++)
            {
                newComponent->addToComponent(selectedStrokes[i]);
                selectedStrokes[i]->highlight(false);
                selectedStrokes[i]->hide();
            }*/
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
        }
    } else if (event->pointerType() == QTabletEvent::Eraser)
    {
		eraseAction(event);	// Let the eraser decide what action to take based on mode.
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
    case UI::Connect :
        switch (status)
        {
        case 0 :
            connectPress(mousePos);
            break;
        case 1:
            connectMove(mousePos);
            break;
        case 2 :
            connectRelease(mousePos);
            break;
        }
        break;
    }
}

bool eventAcceptedByJoint = false;
JointGraphics *touchEventOwner = NULL;

bool PlayGoController::onTouchEventFromView(QTouchEvent *event)
{
    /*if (event->type() == QEvent::TouchBegin)
        {

        } else if (event->type() == QEvent::TouchUpdate)
        {

        } else if (event->type() == QEvent::TouchEnd || event->type() == QEvent::TouchCancel)
        {
            if (eventAcceptedByComponent) {

            }
            eventAcceptedByComponent = false;
            touchEventOwner = NULL;
        }*/

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
                qDebug() << scenePos << tp.scenePos();

                QList<QGraphicsItem*> selectedItems = _scene->items(scenePos, Qt::IntersectsItemBoundingRect,
                                                                    Qt::DescendingOrder, _view->transform());
				JointGraphics* jointGraphics = 0;
				foreach(QGraphicsItem* graphicsitem, selectedItems)
				{
					if (graphicsitem->type() == JointGraphics::Type && graphicsitem->parentItem())
					{
						if (graphicsitem->contains(graphicsitem->mapFromScene(scenePos)))
						{
							jointGraphics = qgraphicsitem_cast<JointGraphics*>(graphicsitem);
							eventAcceptedByJoint = true;
							touchEventOwner = jointGraphics;
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
				eventAcceptedByJoint = false;
				touchEventOwner = 0;
				return true;
			}
			if (event->type() == QEvent::TouchCancel && eventAcceptedByJoint)
			{
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
            qDebug() << d0 << d1 << "SearchView";

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
		qDebug() <<"Extra t" << difference;
		_view->setTransform(t, true);

		event->accept();
        return true;
	}
	return false;
}

void PlayGoController::onGestureEventFromView(QGestureEvent *event)
{
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
			tap_and_hold->setGestureCancelPolicy(QGesture::CancelAllInContext);
			event->accept();
		}
		if (tap_and_hold->state() == Qt::GestureUpdated)
			msg += "Updated>>";
		if (tap_and_hold->state() == Qt::GestureFinished)
			msg += "Finished>>";
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
	_currentMode = GestureSketch;
}

void PlayGoController::setModeLockItem()
{
	connectionModeReset();
	_currentMode  = StaticJoint;
}

void PlayGoController::setModeHingeJoint()
{
    //		QMessageBox::about(NULL, "MSG", "Set to hinge mode");
    connectionModeReset();
    _currentMode = HingeJoint;
}

void PlayGoController::setModeSliderJoint()
{
    connectionModeReset();
    _currentMode = SliderJoint;
}

void PlayGoController::setModeSpringJoint()
{
    //		QMessageBox::about(NULL, "MSG", "Set to spring mode");
    connectionModeReset();
    _currentMode = SpringJoint;
}

void PlayGoController::setModeForce()
{
    //		QMessageBox::about(NULL, "MSG", "Set to force mode");
    connectionModeReset();
    _currentMode = ApplyForce;
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
	QPixmap _pixmap = QPixmap();
	_pixmap.load(imagePath);
	if (_pixmap.width() > 500 || _pixmap.height() > 500)
	{
		_pixmap = _pixmap.scaled(QSize(500,500), Qt::KeepAspectRatio);
	}
	Pixmap* pixmap = new Pixmap(_pixmap, imagePath);
	Component* component = _page->createComponent();
	component->addToComponent(pixmap);

	QPointF scenePos = _view->mapToScene(event->pos());
	component->moveBy(scenePos.x(), scenePos.y());
}

void PlayGoController::startSimulation()
{
    if (_page)
    {
        if (_page->scene() && _page->getPhysicsManager()->debugView)
            _page->getPhysicsManager()->debugView->scene = _page->scene();
        _page->getPhysicsManager()->start(20);
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
