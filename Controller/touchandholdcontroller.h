#ifndef TOUCHANDHOLDCONTROLLER_H
#define TOUCHANDHOLDCONTROLLER_H

#include <QObject>
#include <QAction>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include "cdi2qtwrapper.h"
#include "commonfunctions.h"
#include "physicsjoint.h"

namespace CDI
{
	class PlayGoController;
	class Component;
	class PhysicsJoint;
	class SelectableActions : public QGraphicsEllipseItem
	{
	protected:
		QAction *_action;
		QString _text;

	public:
		enum {Type = UserType + UI_SELECTABLE_ACTIONS };
		static int dim;
	public :
		SelectableActions(QAction *action, QGraphicsItem* parent);

//		QRectF boundingRect() const;

		int type() const { return Type; }

		virtual void trigger();
	};

	class TouchAndHoldController : public QObject
	{
		Q_OBJECT
	protected:
		PlayGoController *_mainController;
		QGraphicsView *_view;
		QAction *_closeOverlayAction;

		QAction *_componentLockAction;
		QAction *_componentUnlockAction;
		QAction *_componentEditAction;		// Edit shape action
		QAction *_componentDisableScaleAction;
		QAction *_componentDeleteAction;
		QAction *_startLayerManager;
		QAction *_addTrackerAction;
		QAction *_removeTrackerAction;
		QAction *_disableComponentCollision;
		QAction *_enableComponentCollision;

		QAction *_jointDeleteAction;
		QAction *_enableMotorAction;
		QAction *_disableMotorAction;
		QAction *_enableLimitsAction;
		QAction *_disableLimitsAction;
		QAction *_editJointSpeedAction;
		QAction *_editJointTorque;

		QGraphicsItemGroup* parentGroup;

		bool _componentEditMode;
		bool _jointEditMode;
		Component* _selectedComponent;
		JointGraphics* _selectedJoint;
		QPointF _scenePos;

	public:
		explicit TouchAndHoldController(QObject *parent = 0);

		PlayGoController *mainController() const;
		void setMainController(PlayGoController *mainController);

		void enableOverlay(Component* component, QPointF scenePos);
		void enableOverlay(JointGraphics* jointGraphics, QPointF scenePos);

		bool handleTapAndHold(QEvent *event);

	protected:
		void handleSelection(QPointF scenePos);

	signals:

	public slots:
		void signalCloseOverlay();
		void signalComponentLockAction();
		void signalComponentUnlockAction();
		void signalComponentEditAction();
		void signalComponentDisableScaleAction();
		void signalComponentDeleteAction();
		void signalEnableCollisionAction();
		void signalDisableCollisionAction();

		void signalJointDelete();
		void signalEnableMotor();
		void signalDisableMotor();
		void signalEnableLimits();
		void signalDisableLimits();
		void signalEditJointSpeed();
		void signalEditJointTorque();

	};
}
#endif // TOUCHANDHOLDCONTROLLER_H
