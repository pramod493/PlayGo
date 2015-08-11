#ifndef TOUCHANDHOLDCONTROLLER_H
#define TOUCHANDHOLDCONTROLLER_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsItemGroup>
#include "cdi2qtwrapper.h"
#include "commonfunctions.h"
#include "physicsjoint.h"
#include "SelectableActions.h"

class QGraphicsView;
class QAction;
namespace CDI
{
	class PlayGoController;
	class Component;
	class PhysicsJoint;
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
		void overlayComponentOptions(Component* component);
		void overlayJointOptions(JointGraphics *jointgraphics);

		bool handleTapAndHold(QEvent *event);

	protected:
		void handleSelection(QPointF scenePos, UI::EventState inputState);

	signals:

	public slots:
		void slotCloseOverlay();
		void slotComponentLockAction();
		void slotComponentUnlockAction();
		void slotComponentEditAction();
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

	};
}
#endif // TOUCHANDHOLDCONTROLLER_H
