#include <QRadialGradient>
#include <QAction>
#include <QTouchEvent>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsObject>

#include "QsLog.h"
#include "PlayGoController.h"
#include "touchandholdcontroller.h"
#include "SelectableActions.h"

namespace CDI
{
	RangeSelector::RangeSelector(QGraphicsItem *graphicsparent)
		: QObject(NULL), QGraphicsPathItem(graphicsparent)
	{
		// No need to set the pen info here. We will do outside
		QPainterPath painterPath;
		int lineLength = 150;
		int radius = 30;
		painterPath.moveTo(0,0);
		painterPath.lineTo(lineLength, 0);
		painterPath.addEllipse(lineLength, -radius, 2*radius, 2*radius);
		setPath(painterPath);

		setAcceptTouchEvents(true);
		setFlag(QGraphicsItem::ItemIgnoresTransformations);

		_angle = 0;
		_itemIsLocked = false;
		_startPos = QPointF();
		_prevPos = QPointF();

		textItem = new QGraphicsSimpleTextItem(this);
		textItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		textItem->setPos (lineLength + 2*radius, 0);
		textItem->setText(QString("0°"));
	}

	int RangeSelector::currentAngle()
	{
		return _angle;
	}

	void RangeSelector::setAngle(int value)
	{
		if (_angle == value) return;
		_angle = value;
		setRotation(_angle);
		textItem->setText(QString::number(_angle) + QString("°"));
	}

	void RangeSelector::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
	{
		QLOG_INFO() << "Mouse press began";
	}

	void RangeSelector::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
	{
		QLOG_INFO() << "Mouse press move";
	}

	void RangeSelector::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
	{
		QLOG_INFO() << "Mouse press release";
	}

	bool RangeSelector::sceneEvent(QEvent* event)
	{
		switch (event->type())
		{
		case QEvent::TouchBegin :
		{
			QTouchEvent* touchEvent = static_cast<QTouchEvent*>(event);
			const QTouchEvent::TouchPoint &tp = touchEvent->touchPoints().first();
			_startPos = tp.scenePos();
			_itemIsLocked = true;
			event->accept();
			return true;
		}
		case QEvent::TouchUpdate :
		{
			if (!_itemIsLocked) return false;	// Item is not locked for some reason

			QTouchEvent* touchEvent = static_cast<QTouchEvent*>(event);
			const QTouchEvent::TouchPoint &tp = touchEvent->touchPoints().first();
			QPointF origin = mapToScene(0,0);
			QPointF currentPos = tp.scenePos() - origin;
			QPointF initialPos = _startPos - origin;

			float deltaAngle = (angleWithX(&currentPos) - angleWithX(&initialPos)) * 180.0f/_PI_;
			setRotation(_angle + deltaAngle);
			textItem->setText(QString::number(static_cast<int>(_angle+deltaAngle)) + QString("°"));
			QLOG_INFO() << "Range Selector angle" << (_angle + deltaAngle);
			event->accept();
			return true;
		}
		case QEvent::TouchEnd :
		{
			if (!_itemIsLocked) return false;	// Item is not locked for some reason

			QTouchEvent* touchEvent = static_cast<QTouchEvent*>(event);
			const QTouchEvent::TouchPoint &tp = touchEvent->touchPoints().first();
			QPointF origin = mapToScene(0,0);
			QPointF currentPos = tp.scenePos() - origin;
			QPointF initialPos = _startPos - origin;

			float deltaAngle = (angleWithX(&currentPos) - angleWithX(&initialPos)) * 180.0f/_PI_;
			if (abs(deltaAngle) > 1.0f) {
				_angle += static_cast<int>(deltaAngle);
				setRotation(_angle);
				emit onAngleChanged();
			} else
			{
				setRotation(_angle);
			}
			textItem->setText(QString::number(_angle) + QString("°"));
			event->accept();
			_itemIsLocked = false;
			return true;
		}
		case QEvent::TouchCancel :
		{
			setRotation(_angle);
			_itemIsLocked = false;
			event->accept();
			return true;	// NOTE - Do we need to accept event/return true in this case?
		}
		default :
			break;
		}
		// handle rest of the events in similat manner
		return QGraphicsPathItem::sceneEvent(event);
	}

	/*-------------------------------------------------------------------------
	------- TouchAndHoldController class Begin -------------------------------*/
	TouchAndHoldController::TouchAndHoldController(QObject *parent) : QObject(parent)
	{
		dpi = 175;

		_mainController = NULL;

		_closeOverlayAction			= new QAction(QIcon(":/images/overlay/close-02.png"), tr("Close"), this);
		_componentLockAction		= new QAction(QIcon(":/images/overlay/lock.png"), tr("Lock"), this);
		_componentUnlockAction		= new QAction(QIcon(":/images/overlay/unlock.png"), tr("Unlock"), this);
		_componentEditAction		= new QAction(QIcon(":/images/overlay/edit.png"), tr("Edit"), this);
		_componentDisableScaleAction = new QAction(QIcon(":/images/overlay/anchor.png"), tr("Disable scaling"), this);
		_componentDeleteAction		= new QAction(QIcon(":/images/overlay/delete-01.png"), tr("Delete"), this);
		_startLayerManager			= new QAction(QIcon(":/images/overlay/layers.png"), tr("Layers"), this);
		_addTrackerAction			= new QAction(QIcon(":/images/overlay/dummy.png"), tr("Add tracker"), this);
		_removeTrackerAction		= new QAction(QIcon(":/images/overlay/dummy.png"), tr("Remove tracker"), this);
		_disableComponentCollision	= new QAction(QIcon(":/images/overlay/dummy.png"), tr("Disable collision"), this);
		_enableComponentCollision	= new QAction(QIcon(":/images/overlay/dummy.png"), tr("Enable collision"), this);

		_jointDeleteAction			= new QAction(QIcon(":/images/overlay/delete-01.png"), tr("Delete"), this);
		_enableMotorAction			= new QAction(QIcon(":/images/overlay/dummy.png"), tr("Add motor"), this);
		_disableMotorAction			= new QAction(QIcon(":/images/overlay/dummy.png"), tr("Remove motor"), this);
		_enableLimitsAction			= new QAction(QIcon(":/images/overlay/dummy.png"), tr("Limit motion"), this);
		_disableLimitsAction		= new QAction(QIcon(":/images/overlay/dummy.png"), tr("Disable limits"), this);
		_editJointSpeedAction		= new QAction(QIcon(":/images/overlay/speed.png"), tr("Change max speed"), this);
		_editJointTorque			= new QAction(QIcon(":/images/overlay/torque.png"), tr("Change force"), this);

		parentGroup = NULL;		//

		_componentEditMode = false;
		_jointEditMode = false;

		_selectedComponent 		= NULL;
		_selectedJoint 			= NULL;
		_selectedPhysicsJoint 	= NULL;

		// Connect actions to slots
		connect(_closeOverlayAction, SIGNAL(triggered()),
				this, SLOT(slotCloseOverlay()));
		connect(_componentLockAction, SIGNAL(triggered()),
				this, SLOT(slotComponentLockAction()));
		connect(_componentUnlockAction, SIGNAL(triggered()),
				this, SLOT(slotComponentUnlockAction()));
		connect(_componentEditAction, SIGNAL(triggered()),
				this, SLOT(slotComponentEditAction()));
		connect(_componentDisableScaleAction, SIGNAL(triggered()),
				this, SLOT(slotComponentDisableScaleAction()));
		connect(_componentDeleteAction, SIGNAL(triggered()),
				this, SLOT(slotComponentDeleteAction()));

		connect(_enableComponentCollision, SIGNAL(triggered()),
				this, SLOT(slotEnableCollisionAction()));
		connect(_disableComponentCollision, SIGNAL(triggered()),
				this, SLOT(slotDisableCollisionAction()));

		connect(_jointDeleteAction, SIGNAL(triggered()),
				this, SLOT(slotJointDelete()));

		connect(_enableMotorAction, SIGNAL(triggered()),
				this, SLOT(slotEnableMotor()));
		connect(_disableMotorAction, SIGNAL(triggered()),
				this, SLOT(slotDisableMotor()));

		connect(_enableLimitsAction, SIGNAL(triggered()),
				this, SLOT(slotEnableLimits()));
		connect(_disableLimitsAction, SIGNAL(triggered()),
				this, SLOT(slotDisableLimits()));

		connect(_editJointSpeedAction, SIGNAL(triggered()),
				this, SLOT(slotEditJointSpeed()));
		connect(_editJointTorque, SIGNAL(triggered()),
				this, SLOT(slotEditJointTorque()));

	}

	PlayGoController *TouchAndHoldController::mainController() const
	{
		return _mainController;
	}

	void TouchAndHoldController::setMainController(PlayGoController *mainController)
	{
		_mainController = mainController;
		_view = static_cast<QGraphicsView*>(_mainController->_view);
	}

	void TouchAndHoldController::enableOverlay(Component *component, QPointF scenePos)
	{
		if (_mainController == NULL || _view == NULL) return;

		_componentEditMode = true;
		_selectedComponent = component;
		_scenePos = scenePos;

		if (parentGroup)
			delete parentGroup;

		float center_radius = 0.25f * dpi;
		float ring_radius = 1.0f * dpi;

		parentGroup = new QGraphicsItemGroup;
		component->scene()->addItem(parentGroup);
		parentGroup->setFlag(QGraphicsItem::ItemIgnoresTransformations);

		bool isItemLocked = (component->isStatic() ? true : false);
		bool isScaleLocked = (component->disableScaling);

		bool itemCollides = (component->groupIndex == 1 ? true : false);

		if (isScaleLocked)
			_componentDisableScaleAction->setText("Enable scaling");
		else
			_componentDisableScaleAction->setText("Disable scaling");

		QGraphicsPathItem* decor =new QGraphicsPathItem(parentGroup);
		decor->setFlag(QGraphicsItem::ItemIgnoresTransformations);

		// Set up colors
		QPen pen = QPen();
		QColor penColor = Qt::green; //QColor(200,200,255);
		pen.setColor(penColor);
		pen.setWidth(3);
		QBrush brush = QBrush(Qt::SolidPattern);
		brush.setColor((penColor.setAlpha(200), penColor.lighter(75)));

		// Set up decor
		QPainterPath path;
		path.addEllipse(QRectF(-center_radius, -center_radius, 2*center_radius, 2*center_radius));
		path.addEllipse(QRectF(-ring_radius, -ring_radius, 2*ring_radius, 2*ring_radius));
		path.moveTo(0, -center_radius); path.lineTo(0, center_radius);
		path.moveTo(-center_radius, 0); path.lineTo(center_radius, 0);
		decor->setPath(path);
		decor->setPen(pen);
		decor->setBrush(brush);

		// Add actual stuff
		float angle = 0;
		float delta = 60;	// deg
		float length = 0.75f * dpi;

		SelectableActions* lockItem = new SelectableActions
				((isItemLocked ? _componentUnlockAction : _componentLockAction), parentGroup);
		lockItem->setPos(length * cos(angle * _PI_/180.0f), length * sin(angle * _PI_/180.0f));
		lockItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		angle += delta;

		SelectableActions* collisionItem = new SelectableActions
				((itemCollides ? _disableComponentCollision : _enableComponentCollision), parentGroup);
		collisionItem->setPos(length * cos(angle * _PI_/180.0f), length * sin(angle * _PI_/180.0f));
		collisionItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		angle += delta;

		SelectableActions* scaleLockItem = new SelectableActions
				(_componentDisableScaleAction, parentGroup);
		scaleLockItem->setPos(length * cos(angle * _PI_/180.0f), length * sin(angle * _PI_/180.0f));
		scaleLockItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		angle += delta;

		SelectableActions* deleteItem = new SelectableActions
				(_componentDeleteAction, parentGroup);
		deleteItem->setPos(length * cos(angle * _PI_/180.0f), length * sin(angle * _PI_/180.0f));
		deleteItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		angle += delta;

		SelectableActions* closeItem = new SelectableActions
				(_closeOverlayAction, parentGroup);
		closeItem->setPos(length * cos(angle * _PI_/180.0f), length * sin(angle * _PI_/180.0f));
		closeItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		angle += delta;

		SelectableActions* layerItem = new SelectableActions
				(_startLayerManager, parentGroup);
		layerItem->setPos(length * cos(angle * _PI_/180.0f), length * sin(angle * _PI_/180.0f));
		layerItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		angle += delta;

		parentGroup->setPos(scenePos);
		parentGroup->setZValue(Z_UIVIEW);
		_mainController->setTapOverride(true);
	}

	void TouchAndHoldController::enableOverlay(JointGraphics *jointGraphics, QPointF scenePos)
	{
		if (_mainController == NULL || _view == NULL) return;

		if (jointGraphics->getPhysicsJoint()->jointType() == e_revoluteJoint)
		{
			_jointEditMode = true;
			_selectedJoint = jointGraphics;
			_scenePos = scenePos;
			if (parentGroup)
				delete parentGroup;

			float center_radius = 0.25f * dpi;
			float ring_radius = 1.0f * dpi;

			parentGroup = new QGraphicsItemGroup;
			_selectedJoint->scene()->addItem(parentGroup);
			parentGroup->setFlag(QGraphicsItem::ItemIgnoresTransformations);

			QGraphicsPathItem* decor =new QGraphicsPathItem(parentGroup);
			decor->setFlag(QGraphicsItem::ItemIgnoresTransformations);

			// Set up colors
			QPen pen = QPen();
			QColor penColor = Qt::blue;
			pen.setColor(penColor);
			pen.setWidth(3);
			QBrush brush = QBrush(Qt::SolidPattern);
			brush.setColor((penColor.setAlpha(150), penColor));

			// Set up decor
			QPainterPath path;
			path.addEllipse(QRectF(-center_radius, -center_radius, 2*center_radius, 2*center_radius));
			path.addEllipse(QRectF(-ring_radius, -ring_radius, 2*ring_radius, 2*ring_radius));
			path.moveTo(0, -center_radius); path.lineTo(0, center_radius);
			path.moveTo(-center_radius, 0); path.lineTo(center_radius, 0);
			decor->setPath(path);
			decor->setPen(pen);
			decor->setBrush(brush);

			// Add actual stuff
			float angle = 0;
			float length = 0.75 * dpi;

			SelectableActions* deleteItem = new SelectableActions
					(_jointDeleteAction, parentGroup);
			deleteItem->setPos(length * cos(angle * _PI_/180.0f), length * sin(angle * _PI_/180.0f));
			deleteItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
			angle += 60;

			SelectableActions* closeItem = new SelectableActions
					(_closeOverlayAction, parentGroup);
			closeItem->setPos(length * cos(angle * _PI_/180.0f), length * sin(angle * _PI_/180.0f));
			closeItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
			angle += 60;

			SelectableActions* enableLimitItem = new SelectableActions
					(_enableLimitsAction, parentGroup);
			enableLimitItem->setPos(length * cos(angle * _PI_/180.0f), length * sin(angle * _PI_/180.0f));
			enableLimitItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
			angle += 60;

			SelectableActions* disableLimitItem = new SelectableActions
					(_disableLimitsAction, parentGroup);
			disableLimitItem->setPos(length * cos(angle * _PI_/180.0f), length * sin(angle * _PI_/180.0f));
			disableLimitItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
			angle += 60;

			parentGroup->setPos(scenePos);
			parentGroup->setZValue(Z_UIVIEW);
			parentGroup->setPanelModality(QGraphicsItem::SceneModal);
			parentGroup->setFlag(QGraphicsItem::ItemIsPanel);
			_mainController->setTapOverride(true);
		}
	}

	void TouchAndHoldController::enableJointLimitsSelection(JointGraphics *jointGraphics, QPointF scenePos)
	{
		if (_mainController == NULL || _view == NULL) return;
		if (parentGroup) delete parentGroup;

		float center_radius = 0.15f * dpi;

		parentGroup = new QGraphicsItemGroup;
		_mainController->_scene->addItem(parentGroup);

		parentGroup->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
		QGraphicsPathItem* decor = new QGraphicsPathItem(parentGroup);
		decor->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);

		// Set up colors
		QPen pen = QPen();
		QColor penColor = QColor(100,100,100); //QColor(200,200,255);
		pen.setColor(penColor);
		pen.setWidth(3);
		QBrush brush = QBrush(Qt::SolidPattern);
		brush.setColor((penColor.setAlpha(245), penColor.lighter(20)));

		// Set up decor
		QPainterPath path;
		path.addEllipse(QRectF(-center_radius, -center_radius, 2*center_radius, 2*center_radius));
		path.moveTo(0, -center_radius); path.lineTo(0, center_radius);
		path.moveTo(-center_radius, 0); path.lineTo(center_radius, 0);
		decor->setPath(path);
		decor->setPen(pen);
		decor->setBrush(brush);

		pen.setColor(Qt::black);
		pen.setWidth(5);

		RangeSelector* startSelector = new RangeSelector(parentGroup);
		startSelector->setPen(pen);
		startSelector->setBrush(QBrush(QColor(255,100,100)));
		startSelector->setAngle(0);

		RangeSelector* endSelector = new RangeSelector(parentGroup);
		endSelector->setPen(pen);
		endSelector->setBrush(QBrush(QColor(100,100,255)));
		endSelector->setAngle(45);

		parentGroup->setPos(scenePos);
		parentGroup->setZValue(Z_UIVIEW);
	}

	void TouchAndHoldController::overlayComponentOptions(Component* component)
	{
		Q_UNUSED(component)
	}

	void TouchAndHoldController::overlayJointOptions(JointGraphics *jointgraphics)
	{
		Q_UNUSED(jointgraphics)
	}

	bool TouchAndHoldController::handleTapAndHold(QEvent *event)
	{
		switch(event->type())
		{
		case QEvent::TouchBegin :
		{
			event->accept();
			QTouchEvent* touchEvent = static_cast<QTouchEvent*>(event);
			QList<QTouchEvent::TouchPoint> touchpoints = touchEvent->touchPoints();
			const QTouchEvent::TouchPoint &tp = touchpoints.first();
			QPointF scenePos = _view->mapToScene(tp.pos().toPoint());
			handleSelection(scenePos, UI::Began);
			return true;
		}
		case QEvent::TouchUpdate :
		{
			QTouchEvent* touchEvent = static_cast<QTouchEvent*>(event);
			QList<QTouchEvent::TouchPoint> touchpoints = touchEvent->touchPoints();
			const QTouchEvent::TouchPoint &tp = touchpoints.first();
			QPointF scenePos = _view->mapToScene(tp.pos().toPoint());
			handleSelection(scenePos, UI::Update);
			return true;
		}
		case QEvent::TouchCancel :
		{
			QTouchEvent* touchEvent = static_cast<QTouchEvent*>(event);
			QList<QTouchEvent::TouchPoint> touchpoints = touchEvent->touchPoints();
			const QTouchEvent::TouchPoint &tp = touchpoints.first();
			QPointF scenePos = _view->mapToScene(tp.pos().toPoint());
			handleSelection(scenePos, UI::Cancel);
			return true;
		}
		case QEvent::TouchEnd :
		{
			QTouchEvent* touchEvent = static_cast<QTouchEvent*>(event);
			QList<QTouchEvent::TouchPoint> touchpoints = touchEvent->touchPoints();
			const QTouchEvent::TouchPoint &tp = touchpoints.first();
			QPointF scenePos = _view->mapToScene(tp.pos().toPoint());
			handleSelection(scenePos, UI::End);
			return true;
		}
		case QEvent::MouseButtonDblClick :
		{
			break;
		}
		case QEvent::MouseButtonPress :
		{
			QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
			handleSelection(_view->mapToScene(mouseEvent->pos()), UI::Began);
			return true;
		}
		case QEvent::MouseMove :
		{
			return true;
			break;
		}
		case QEvent::MouseButtonRelease :
		{
			QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
			handleSelection(_view->mapToScene(mouseEvent->pos()), UI::End);
		}
		case QEvent::TabletPress :
		case QEvent::TabletMove :
		{
				QTabletEvent* tabletEvent = static_cast<QTabletEvent*>(event);
				handleSelection(_view->mapToScene(tabletEvent->pos()), UI::Update);
				return true;
		}
		case QEvent::TabletRelease :
		{
			QTabletEvent* tabletEvent = static_cast<QTabletEvent*>(event);
			handleSelection(_view->mapToScene(tabletEvent->pos()), UI::End);
			return true;
		}
		case QEvent::Gesture :
		{
			return true;
			break;
		}
		default :
			break;
		}
		return false;
	}

	void TouchAndHoldController::handleSelection(QPointF scenePos, UI::EventState inputState)
	{
		if (!(_componentEditMode || _jointEditMode))
		{
			slotCloseOverlay();
			return;
		}

		switch (inputState)
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
				QGraphicsScene *scene = _mainController->_scene;
				QList<QGraphicsItem*> items = scene->items(scenePos, Qt::IntersectsItemBoundingRect,
														   Qt::DescendingOrder,	_view->transform());
				for (QList<QGraphicsItem*>::const_iterator it = items.constBegin();
					 it != items.constEnd(); ++it)
				{
					QGraphicsItem* graphicsitem = (*it);
					if (graphicsitem->type() == SelectableActions::Type)
					{
						SelectableActions* selectableAction =
								qgraphicsitem_cast<SelectableActions*>(graphicsitem);
						selectableAction->trigger();
						return;
					}
				}
			}
			case UI::Cancel :
			{
				// This might not be meaningful in many cases
			}
		}
	}

	void TouchAndHoldController::slotCloseOverlay()
	{
		if (parentGroup)
			delete parentGroup;

		// Clear all variables
		parentGroup = 0;	// Clean up
		_componentEditMode = false;
		_jointEditMode = false;
		_selectedComponent = 0;
		_selectedJoint = 0;

		_mainController->setTapOverride(false);
	}

	void TouchAndHoldController::slotComponentLockAction()
	{
		if (_componentEditMode)
		{
			_selectedComponent->setStatic(true);
			slotCloseOverlay();
		}
	}

	void TouchAndHoldController::slotComponentUnlockAction()
	{
		if (!_componentEditMode) return;
		_selectedComponent->setStatic(false);
		slotCloseOverlay();
	}

	void TouchAndHoldController::slotComponentEditAction()
	{
			if (!_componentEditMode) return;
			// TODO - launch edit window
	}

	void TouchAndHoldController::slotComponentDisableScaleAction()
	{
		if (!_componentEditMode) return;
		_selectedComponent->disableScaling = !(_selectedComponent->disableScaling);
		slotCloseOverlay();
	}

	void TouchAndHoldController::slotComponentDeleteAction()
	{
		if (!_componentEditMode) return;
		_mainController->_page->destroyComponent(_selectedComponent);
		slotCloseOverlay();
	}

	void TouchAndHoldController::slotEnableCollisionAction()
	{
		if (_componentEditMode)
		{
			_selectedComponent->groupIndex = 1;
			_selectedComponent->onCollisionBitsUpdate();
			slotCloseOverlay();
		}
	}

	void TouchAndHoldController::slotDisableCollisionAction()
	{
		if (_componentEditMode)
		{
			_selectedComponent->groupIndex = -1;
			_selectedComponent->onCollisionBitsUpdate();
			slotCloseOverlay();
		}
	}

	void TouchAndHoldController::slotJointDelete()
	{
		if (_jointDeleteAction)
		{
			_mainController->_page->getPhysicsManager()->deleteJoint(_selectedJoint->getPhysicsJoint());
			delete _selectedJoint;
			slotCloseOverlay();
		}
	}

	void TouchAndHoldController::slotEnableMotor()
	{

	}

	void TouchAndHoldController::slotDisableMotor()
	{

	}

	void TouchAndHoldController::slotEnableLimits()
	{
		if (_jointEditMode)
		{
			if (parentGroup) delete parentGroup;
			parentGroup = 0;	// Clean up
			enableJointLimitsSelection(_selectedJoint, _scenePos);
		}
	}

	void TouchAndHoldController::slotDisableLimits()
	{

	}

	void TouchAndHoldController::slotEditJointSpeed()
	{

	}

	void TouchAndHoldController::slotEditJointTorque()
	{

	}
}
