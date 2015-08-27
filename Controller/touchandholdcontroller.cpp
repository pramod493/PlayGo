#include <QRadialGradient>
#include <QAction>
#include <QTouchEvent>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsObject>

#include "QsLog.h"
#include "PlayGoController.h"
#include "touchandholdcontroller.h"
#include "SelectableActions.h"
#include "pinjointlimitselector.h"

namespace CDI
{
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
		_componentCopyAction		= new QAction(QIcon(":/images/overlay/layers.png"), tr("Copy"), this);
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

		parentGroup = nullptr;		//

		_componentEditMode	= false;
		_jointEditMode		= false;

		_selectedComponent 		= nullptr;
		_selectedJoint 			= nullptr;
		_jointParamsChanged		= false;

		// Connect actions to slots
		// Close
		connect(_closeOverlayAction, SIGNAL(triggered()),
				this, SLOT(slotCloseOverlay()));
		// Lock
		connect(_componentLockAction, SIGNAL(triggered()),
				this, SLOT(slotComponentLockAction()));
		// Unlock
		connect(_componentUnlockAction, SIGNAL(triggered()),
				this, SLOT(slotComponentUnlockAction()));
		// Edit
		connect(_componentEditAction, SIGNAL(triggered()),
				this, SLOT(slotComponentEditAction()));
		// Copy
		connect(_componentCopyAction, SIGNAL(triggered()),
				this, SLOT(slotComponentCopyAction()));
		// Disable scaling
		connect(_componentDisableScaleAction, SIGNAL(triggered()),
				this, SLOT(slotComponentDisableScaleAction()));
		// Delete
		connect(_componentDeleteAction, SIGNAL(triggered()),
				this, SLOT(slotComponentDeleteAction()));
		// Enable collision
		connect(_enableComponentCollision, SIGNAL(triggered()),
				this, SLOT(slotEnableCollisionAction()));
		// Disable collision
		connect(_disableComponentCollision, SIGNAL(triggered()),
				this, SLOT(slotDisableCollisionAction()));

		// Joint delete
		connect(_jointDeleteAction, SIGNAL(triggered()),
				this, SLOT(slotJointDelete()));
		// Motor enable
		connect(_enableMotorAction, SIGNAL(triggered()),
				this, SLOT(slotEnableMotor()));
		// Motor disable
		connect(_disableMotorAction, SIGNAL(triggered()),
				this, SLOT(slotDisableMotor()));
		// Limits enable
		connect(_enableLimitsAction, SIGNAL(triggered()),
				this, SLOT(slotEnableLimits()));
		// Limits disable
		connect(_disableLimitsAction, SIGNAL(triggered()),
				this, SLOT(slotDisableLimits()));
		// Change speed
		connect(_editJointSpeedAction, SIGNAL(triggered()),
				this, SLOT(slotEditJointSpeed()));
		// Change torque - Unused
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

		auto motorparamsupdate = [&]()
		{
			if (_jointEditMode && _selectedJoint != NULL)
			{
				bool enableMotor = false;
				float motorSpeed = 0;
				float motorTorque = 0;
				_mainController->getMotorParams(&enableMotor, &motorSpeed, &motorTorque);

				if (auto pinjoint = dynamic_cast<cdPinJoint*>(_selectedJoint))
				{
					//b2RevoluteJoint* revoluteJoint = static_cast<b2RevoluteJoint*>(physicsJoint->joint());
					auto def = pinjoint->jointDef();

					def->enableMotor = enableMotor;
					def->motorSpeed = motorSpeed * TO_RAD_SEC_FROM_RPM;
					def->maxMotorTorque = motorTorque;
				} else if  (auto sliderjoint = dynamic_cast<cdSliderJoint*>(_selectedJoint))
				{
					//b2RevoluteJoint* revoluteJoint = static_cast<b2RevoluteJoint*>(physicsJoint->joint());
					auto def = sliderjoint->jointDef();

					def->enableMotor = enableMotor;
					def->motorSpeed = motorSpeed;
					def->maxMotorForce = motorTorque;
				}
				_jointParamsChanged = true;
			}
		};

		connect(_mainController->motorTorque, &QLineEdit::textChanged, motorparamsupdate);
		connect(_mainController->motorSpeed, &QLineEdit::textChanged, motorparamsupdate);
		connect(_mainController->enableMotorCheckbox, &QCheckBox::toggled, motorparamsupdate);

		// create connections with tool bar changes
		/* No need of this one
		 * connect(_mainController->motorTorque, SIGNAL(textChanged(QString)),
				this, SLOT(slotMotorParamsChange()));
		connect(_mainController->motorSpeed, SIGNAL(textChanged(QString)),
				this, SLOT(slotMotorParamsChange()));
		connect(_mainController->enableMotorCheckbox, SIGNAL(toggled(bool)),
				this, SLOT(slotMotorParamsChange()));*/

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

		SelectableActions* copyItem = new SelectableActions
				(_componentCopyAction, parentGroup);
		copyItem->setPos(length * cos(angle * _PI_/180.0f), length * sin(angle * _PI_/180.0f));
		copyItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		angle += delta;

		/*SelectableActions* layerItem = new SelectableActions
				(_startLayerManager, parentGroup);
		layerItem->setPos(length * cos(angle * _PI_/180.0f), length * sin(angle * _PI_/180.0f));
		layerItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		angle += delta;*/

		parentGroup->setPos(scenePos);
		parentGroup->setZValue(Z_UIVIEW);
		_mainController->setTapOverride(true);
	}

	void TouchAndHoldController::enableOverlay(cdJoint *physicsJoint, QPointF scenePos)
	{
		if (_mainController == nullptr || _view == nullptr) return;

		_scenePos = scenePos;
		if (parentGroup) delete parentGroup;

		if (physicsJoint->jointType() == e_revoluteJoint)
		{
			_jointEditMode = true;
			_selectedJoint = physicsJoint;

			enablePinJointOverlay(dynamic_cast<cdPinJoint*>(physicsJoint), scenePos);
		} else if (physicsJoint->jointType() == e_prismaticJoint)
		{
			_jointEditMode = true;
			_selectedJoint = physicsJoint;

			enableSliderJointOverlay(dynamic_cast<cdSliderJoint*>(physicsJoint), scenePos);
		}
	}

	void TouchAndHoldController::enableJointLimitsSelection(cdPinJoint *physicsJoint, QPointF scenePos)
	{
		if (_mainController == NULL || _view == NULL) return;
		// This will enable us to call this function directly from main controller

		_mainController->setTapOverride(true);

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

		/*PinJointLimitsSelector * limiter = */
		new PinJointLimitsSelector (physicsJoint, parentGroup);

		// Close button
		SelectableActions* closeItem = new SelectableActions
				(_closeOverlayAction, parentGroup);
		closeItem->setPos(0,0);
		closeItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);

		parentGroup->setPos(scenePos);
		parentGroup->setZValue(Z_UIVIEW);
	}

	void TouchAndHoldController::overlayComponentOptions(Component* component)
	{
		Q_UNUSED(component)
	}

	void TouchAndHoldController::overlayJointOptions(cdJoint *physicsJoint)
	{
		Q_UNUSED(physicsJoint)
	}

	bool TouchAndHoldController::handleTapAndHold(QEvent *event)
	{
		return false;		// Nothing to be done here

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
		Q_UNUSED (scenePos)
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
				break;
			}
			case UI::Cancel :
			{
				// This might not be meaningful in many cases
			}
		}
	}

	void TouchAndHoldController::enablePinJointOverlay(cdPinJoint *pinjoint, QPointF scenePos)
	{

		float center_radius = 0.25f * dpi;
		float ring_radius = 1.0f * dpi;

		parentGroup = new QGraphicsItemGroup;
		pinjoint->scene()->addItem(parentGroup);
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

		if (pinjoint->isLimitsEnabled())
		{	// already disabled
			SelectableActions* disableLimitItem = new SelectableActions
					(_disableLimitsAction, parentGroup);
			disableLimitItem->setPos(length * cos(angle * _PI_/180.0f), length * sin(angle * _PI_/180.0f));
			disableLimitItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
			angle += 60;
		}
		parentGroup->setPos(scenePos);
		parentGroup->setZValue(Z_UIVIEW);
		parentGroup->setPanelModality(QGraphicsItem::SceneModal);
		parentGroup->setFlag(QGraphicsItem::ItemIsPanel);
		_mainController->setTapOverride(true);

		// Get the settings and use these to update the toolbar
		auto jointDef = pinjoint->jointDef();
		// Setting up the correct values in the toolbar
		// \todo Use the converted values. Avoid calling the jointDef.
		_mainController->setMotorParams(jointDef->enableMotor,
										jointDef->motorSpeed * TO_RPM_FROM_RAD_SEC,
										jointDef->maxMotorTorque);
	}

	void TouchAndHoldController::enableSliderJointOverlay(cdSliderJoint *sliderjoint, QPointF scenePos)
	{
		float center_radius = 0.25f * dpi;
		float ring_radius = 1.0f * dpi;

		parentGroup = new QGraphicsItemGroup;
		sliderjoint->scene()->addItem(parentGroup);
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

		/* Limit the options for slider joint to delete and close
		 *
		SelectableActions* enableLimitItem = new SelectableActions
				(_enableLimitsAction, parentGroup);
		enableLimitItem->setPos(length * cos(angle * _PI_/180.0f), length * sin(angle * _PI_/180.0f));
		enableLimitItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		angle += 60;

		// already disabled
		SelectableActions* disableLimitItem = new SelectableActions
				(_disableLimitsAction, parentGroup);
		disableLimitItem->setPos(length * cos(angle * _PI_/180.0f), length * sin(angle * _PI_/180.0f));
		disableLimitItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		angle += 60;
		*/
		parentGroup->setPos(scenePos);
		parentGroup->setZValue(Z_UIVIEW);
		parentGroup->setPanelModality(QGraphicsItem::SceneModal);
		parentGroup->setFlag(QGraphicsItem::ItemIsPanel);
		_mainController->setTapOverride(true);

		// Get the settings and use these to update the toolbar
		auto jointDef = sliderjoint->jointDef();
		// Setting up the correct values in the toolbar
		// \todo Use the converted values. Avoid calling the jointDef.
		_mainController->setMotorParams(jointDef->enableMotor,
										jointDef->motorSpeed * TO_RPM_FROM_RAD_SEC,
										jointDef->maxMotorForce);
	}

	void TouchAndHoldController::slotCloseOverlay()
	{
		if (_jointEditMode && _jointParamsChanged && (_selectedJoint != nullptr))
		{
			// update the joint parameters
			_jointParamsChanged = false;
			_selectedJoint->updateJoint();
		}

		if (parentGroup)
			delete parentGroup;

		// Clear all variables
		parentGroup = nullptr;	// Clean up
		_componentEditMode = false;
		_jointEditMode = false;
		_selectedComponent = nullptr;
		_selectedJoint = nullptr;
		_jointParamsChanged = false;

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

	void TouchAndHoldController::slotComponentCopyAction()
	{
		if (_componentEditMode && _selectedComponent != nullptr)
		{
			auto newComponent =
					_mainController->_page->createComponent(_selectedComponent);
			/*QLOG_INFO() << "Component copied!" << _selectedComponent->id().toString()
						<< newComponent->id().toString();*/
			newComponent->moveBy(20,20);
		}
		slotCloseOverlay();
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
			delete _selectedJoint;	// it takes care of stuff
			_selectedJoint = nullptr;
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
		if (_jointEditMode && _selectedJoint->jointType() == e_revoluteJoint)
		{
			if (parentGroup) delete parentGroup;
			parentGroup = 0;	// Clean up
			enableJointLimitsSelection(static_cast<cdPinJoint*>(_selectedJoint), _scenePos);
		} else
		{
			slotCloseOverlay();
		}
	}

	void TouchAndHoldController::slotDisableLimits()
	{
		if (_jointEditMode && _selectedJoint->jointType() == e_revoluteJoint)
		{
			auto pinjoint = dynamic_cast<cdPinJoint*>(_selectedJoint);
			pinjoint->joint()->EnableLimit(false);
			pinjoint->jointDef()->enableLimit = false;
		}
		slotCloseOverlay();
	}

	void TouchAndHoldController::slotEditJointSpeed()
	{

	}

	void TouchAndHoldController::slotEditJointTorque()
	{

	}

	void TouchAndHoldController::slotMotorParamsChange()
	{
		QMessageBox::about(nullptr, "Wrong place", "Noting sud call @slotMotorParamsChange");
		/*if (_jointEditMode && _selectedJoint != NULL)
		{
			bool enableMotor = false;
			float motorSpeed = 0;
			float motorTorque = 0;
			_mainController->getMotorParams(&enableMotor, &motorSpeed, &motorTorque);

			if (auto pinjoint = dynamic_cast<cdPinJoint*>(_selectedJoint))
			{
				//b2RevoluteJoint* revoluteJoint = static_cast<b2RevoluteJoint*>(physicsJoint->joint());
				auto def = pinjoint->jointDef();

				def->enableMotor = enableMotor;
				def->motorSpeed = motorSpeed * TO_RAD_SEC_FROM_RPM;
				def->maxMotorTorque = motorTorque;
			} else if  (auto sliderjoint = dynamic_cast<cdSliderJoint*>(_selectedJoint))
			{
				//b2RevoluteJoint* revoluteJoint = static_cast<b2RevoluteJoint*>(physicsJoint->joint());
				auto def = sliderjoint->jointDef();

				def->enableMotor = enableMotor;
				def->motorSpeed = motorSpeed;
				def->maxMotorForce = motorTorque;
			}
			_jointParamsChanged = true;
		}*/
	}
}
