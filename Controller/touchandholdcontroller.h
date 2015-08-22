#ifndef TOUCHANDHOLDCONTROLLER_H
#define TOUCHANDHOLDCONTROLLER_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include <QGraphicsView>

#include "cdi2qtwrapper.h"
#include "commonfunctions.h"
#include "physicsjoint.h"
#include "SelectableActions.h"

class QGraphicsObject;
class QGraphicsPathItem;
class QGraphicsSimpleTextItem;
class QAction;

namespace CDI
{
	class PlayGoController;
	class Component;
	class PhysicsJoint;
	/**
	 * @brief The TouchAndHoldController class displays the on screen menu overlaid
	 * on the selected entity
	 */
	class TouchAndHoldController : public QObject
	{
		Q_OBJECT
	public:
		int dpi;
	protected:
		PlayGoController *_mainController;
		QGraphicsView *_view;
		QAction *_closeOverlayAction;

		QAction *_componentLockAction;
		QAction *_componentUnlockAction;
		QAction *_componentEditAction;		// Edit shape action
		QAction *_componentCopyAction;
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
		PhysicsJoint* _selectedJoint;
		bool _jointParamsChanged;
		QPointF _scenePos;

	public:
		explicit TouchAndHoldController(QObject *parent = 0);

		PlayGoController *mainController() const;
		void setMainController(PlayGoController *mainController);

		/**
		 * @brief Display overlay with options related to component
		 * @param component
		 * @param scenePos
		 */
		void enableOverlay(Component* component, QPointF scenePos);

		/**
		 * @brief Dispay overlay with options related to joint
		 * @param physicsJoint PhysicsJoint
		 * @param scenePos selection position
		 */
		void enableOverlay(PhysicsJoint *physicsJoint, QPointF scenePos);

		void enableJointLimitsSelection(PhysicsJoint *physicsJoint, QPointF scenePos);

		void overlayComponentOptions(Component* component);
		void overlayJointOptions(PhysicsJoint *physicsJoint);

		bool handleTapAndHold(QEvent *event);

	protected:
		void handleSelection(QPointF scenePos, UI::EventState inputState);

	signals:

	public slots:
		void slotCloseOverlay();
		void slotComponentLockAction();
		void slotComponentUnlockAction();
		void slotComponentEditAction();
		void slotComponentCopyAction();
		void slotComponentDisableScaleAction();
		void slotComponentDeleteAction();
		void slotEnableCollisionAction();
		void slotDisableCollisionAction();

		void slotJointDelete();
		void slotEnableMotor();
		void slotDisableMotor();
		void slotEnableLimits();
		void slotDisableLimits();
		void slotEditJointSpeed();
		void slotEditJointTorque();

		void slotMotorParamsChange();
	};
}
#endif // TOUCHANDHOLDCONTROLLER_H
