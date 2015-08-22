#include "connectcontroller.h"
#include "PlayGoController.h"
#include "sketchscene.h"
#include "sketchview.h"
#include "cdiwindow.h"
#include <QToolBar>
#include <QGraphicsLineItem>
#include "forceitem.h"
#include <QMessageBox>
#include <box2dworld.h>

namespace CDI
{
	ConnectController::ConnectController(PlayGoController *parent)
		: QObject(parent)
	{
		_page = parent->page();
		_scene = parent->scene();
		_view = parent->view();
		_parentController = parent;
		_mainWindow = parent->_toplevelWindow;
		_currentMode = HingeJoint;
		createToolbar();

		// Initialize and load gesture recognizer
		sketchRecognizer = new PDollarRecognizer(this);		// Will make sure that this gets deleted with the object
		QString gestureDir = getHomeDirectory() + "/gestures/";
		sketchRecognizer->loadPDRTemplates(gestureDir);

		// List of user strokes
		_tmpStrokes = QList<PenStroke*>();

		_currentStroke = NULL;

		connect(sketchRecognizer, SIGNAL(onGestureDetect(QString,float)),
				this, SLOT(onGestureRecognized(QString,float)));
	}

	ConnectController::~ConnectController()
	{

	}

	ForceGraphicsItem* forceLine = NULL;
	void ConnectController::onPress(QPointF scenePos)
	{

		switch (_currentMode)
		{
		case GestureSketch :
		{
			if (_currentStroke)
			{
				QLOG_WARN() << "Current stroke should have been NULL.";
				delete _currentStroke;
				_currentStroke = NULL;
			}

			_currentStroke = new PenStroke();
			_scene->addItem(_currentStroke);
			_currentStroke->push_point(scenePos);	// No transformation for this one
			QPen strokePen = QPen();
			strokePen.setWidth(3);
			strokePen.setColor(Qt::blue);
			_currentStroke->setPen(strokePen);
			break;
		}
		case HingeJoint :
		{
			// Check if the point lies on any point
			QList<QGraphicsItem*> selectedItems = _scene->items(scenePos, Qt::IntersectsItemBoundingRect);
			QList<Component*> itemsToUse;
			if (selectedItems.size() == 0) return;
			foreach (QGraphicsItem* graphicsitem, selectedItems)
			{
				if (graphicsitem->parentItem() == NULL) continue;
				if (graphicsitem->parentItem()->type() != Component::Type) continue;
				if (graphicsitem->type() != Pixmap::Type) continue;	// Replace to accomodate other widgets

				if (graphicsitem->contains(graphicsitem->mapFromScene(scenePos)))
				{
					Component* c = qgraphicsitem_cast<Component*>(graphicsitem->parentItem());
					itemsToUse.push_back(c);
				}
			}

			QLOG_INFO() << itemsToUse.size() << "components selected";
			if (itemsToUse.size() == 0)
			{
				QLOG_INFO() << "No valid component found. Selection size = " << selectedItems.size();
			}

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
					QMessageBox::warning(NULL, "ERROR", "Both selected comonents have same ID");
				}

				b2RevoluteJointDef revolutJointDef;
				revolutJointDef.bodyA = c1->physicsBody();
				//c1->physicsBody()->SetType(b2_kinematicBody);
				revolutJointDef.bodyB = c2->physicsBody();
				revolutJointDef.collideConnected = false;

				QPointF localPos1 = c1->mapFromScene(scenePos);
				QPointF localPos2 = c2->mapFromScene(scenePos);
				float physicsScale = getPhysicsScale();
				revolutJointDef.localAnchorA.Set(localPos1.x()/physicsScale, localPos1.y()/physicsScale);
				revolutJointDef.localAnchorB.Set(localPos2.x()/physicsScale, localPos2.y()/physicsScale);

				revolutJointDef.enableLimit = false;

				revolutJointDef.enableMotor = true;
				revolutJointDef.maxMotorTorque = 2000;
				revolutJointDef.motorSpeed = 1000*3.14f;	// 50 rot

				//_page->getPhysicsManager()->_b2World->CreateJoint(&revolutJointDef);

				QGraphicsEllipseItem* marker = new QGraphicsEllipseItem(c1);
				marker->setPos(localPos1);
				float radius = 10;
				marker->setRect(-radius, -radius, radius, radius);
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
//				forceLine = new ForceGraphicsItem(0,0,0,0);
//				forceLine->setTransform(t);
//				_scene->addItem(forceLine);
//				component->addToComponent(forceLine);
				return;
			}
			break;
		}
		}
	}

	void ConnectController::onMove(QPointF scenePos)
	{
		switch (_currentMode)
		{
		case GestureSketch :
		{
			if (_currentStroke)
				_currentStroke->push_point(scenePos);
			break;
		}
		case HingeJoint :
			break;
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

		return;

		return;


	}

	void ConnectController::onRelease(QPointF scenePos)
	{

		switch (_currentMode)
		{
		case GestureSketch :
		{
			if (_currentStroke)
			{
				_currentStroke->push_point(scenePos);

				sketchRecognizer->addStroke(_currentStroke->points);
				_tmpStrokes.push_back(_currentStroke);
				sketchRecognizer->gbRecognize();

				_currentStroke = NULL;
			}
			break;
		}
		case HingeJoint :
			break;
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

						QLOG_INFO() << "Force output" << forceorigin << forcevector;
					}
				}
			}
			delete forceLine;
			forceLine = NULL;
			break;
		}
		}
	}

	void ConnectController::showToolbar()
	{
		if (connectionOptionsToolbar) connectionOptionsToolbar->show();
	}

	void ConnectController::hideToolbar()
	{
		if (connectionOptionsToolbar) connectionOptionsToolbar->hide();
	}

	void ConnectController::createToolbar()
	{
		// Create actions
		QActionGroup * actionGroup = new QActionGroup(this);
		QAction* scribbleModeSelection = new QAction(QIcon(":/images/scribble.png"), tr("Recognizer mode"), actionGroup);
		scribbleModeSelection->setCheckable(true);
		scribbleModeSelection->setChecked(true);

		QAction* hingeSelectAction = new QAction(QIcon(":/images/joints/hinge-joint.png"), tr("Create hinge joint"), actionGroup);
		hingeSelectAction->setCheckable(true);
		hingeSelectAction->setChecked(false);

		QAction* springSelectAction = new QAction(QIcon(":/images/joints/spring-joint.png"), tr("Connect using springs"), actionGroup);
		springSelectAction->setCheckable(true);
		springSelectAction->setChecked(false);

		QAction* forceSelectAction = new QAction(QIcon(":/images/joints/force.png"), tr("Apply force"), actionGroup);
		forceSelectAction->setCheckable(true);
		forceSelectAction->setChecked(false);

		connectionOptionsToolbar = new QToolBar(tr("Connect mode toolbar"), _mainWindow);

		connectionOptionsToolbar->addAction(scribbleModeSelection);
		connectionOptionsToolbar->addAction(hingeSelectAction);
		connectionOptionsToolbar->addAction(springSelectAction);
		connectionOptionsToolbar->addAction(forceSelectAction);

		connectionOptionsToolbar->setIconSize(QSize(48,48));

		_mainWindow->addToolBar(Qt::TopToolBarArea, connectionOptionsToolbar);

		connect(scribbleModeSelection, SIGNAL(triggered()),
				this, SLOT(setModeScribble()));
		connect(hingeSelectAction, SIGNAL(triggered()),
				this, SLOT(setModeHingeJoint()));
		connect(springSelectAction, SIGNAL(triggered()),
				this, SLOT(setModeSpringJoint()));
		connect(forceSelectAction, SIGNAL(triggered()),
				this, SLOT(setModeForce()));

	}

	void ConnectController::onGestureReceive(QGraphicsItem *graphicsitem)
	{
		Q_UNUSED(graphicsitem)
	}

	void ConnectController::onTabletEvent(QTabletEvent *event)
	{
		QPointF sceneMappedPos = _view->mapToScene(event->pos());
		switch (event->type())
		{
		case QEvent::TabletPress :
			onPress(sceneMappedPos);
			break;
		case QEvent::TabletMove :
			onMove(sceneMappedPos);
			break;
		case QEvent::TabletRelease :
			onRelease(sceneMappedPos);
			break;
		}
	}

	void ConnectController::onMouseEventFromScene(QGraphicsSceneMouseEvent *event, int eventStatus)
	{
		switch (eventStatus)
		{
		case 0 :
			onPress(event->scenePos());
			break;
		case 1:
			onMove(event->scenePos());
			break;
		case 2 :
			onRelease(event->scenePos());
			break;
		}
	}

	void ConnectController::setModeScribble()
	{
		_currentMode = GestureSketch;
	}

	void ConnectController::setModeHingeJoint()
	{
		QMessageBox::about(NULL, "MSG", "Set to hinge mode");
		_currentMode = HingeJoint;
	}

	void ConnectController::setModeSpringJoint()
	{
		QMessageBox::about(NULL, "MSG", "Set to spring mode");
		_currentMode = SpringJoint;
	}

	void ConnectController::setModeForce()
	{
		QMessageBox::about(NULL, "MSG", "Set to force mode");
		_currentMode = ApplyForce;
	}

	void ConnectController::onGestureRecognized(QString gestureName, float gestureScore)
	{
		if (gestureScore > 0.1)
		{
			// Sure shot way to recognize

		}
		QMessageBox::about(NULL, "Gesture recognized", gestureName);

		qDeleteAll(_tmpStrokes);
		_tmpStrokes.clear();
		sketchRecognizer->clean();
	}

	void ConnectController::clean()
	{
		qDeleteAll(_tmpStrokes);
		_tmpStrokes.clear();

		if (sketchRecognizer) sketchRecognizer->clean();
	}
}
