#ifndef PINJOINTLIMITSELECTOR_H
#define PINJOINTLIMITSELECTOR_H

#include <QObject>
#include <QGraphicsPathItem>
#include <QGraphicsItemGroup>
#include <QGraphicsSimpleTextItem>
#include <QEvent>
#include <QTouchEvent>
#include <QGraphicsSceneMouseEvent>

#include "cdi2qtwrapper.h"
#include "commonfunctions.h"
#include "physicsjoint.h"
#include "component.h"
#include "SelectableActions.h"
namespace CDI
{
	/**
	 * @brief The RangeDialHandle class repesents a QGraphicsItem object which can be
	 * rotated around its origin. It allows a full 360 deg rotation and does not
	 * enforce any limits on the motion
	 */
	class RangeDialHandle : public QObject, public QGraphicsPathItem
	{
		Q_OBJECT
		int _angle;				/**< Angle of the handle (in degrees) */
		float _tmpAngle;
		bool _itemIsLocked;		/**< Item is currently being transformed */
		QPointF _startPos;		/**< start position of drag */
		QPointF _prevPos;		/**< previous position of drag */
		QGraphicsSimpleTextItem *textItem;

	public:
		// NOTE - Deletion is handled by QGraphicsItem. QObject hierarchy not used for deletion
		RangeDialHandle(int handleLength = 150, int handleRadius = 30, QGraphicsItem  *parent = NULL);
		virtual ~RangeDialHandle() {}

		/**
		 * @brief Returns the current angle value
		 * @return Angle of the selector
		 */
		int currentAngle();

		/**
		 * @brief setText updates the text displayed next to handle
		 * @param text Text to be shown
		 * @remarks This might be overriden by the object itself
		 */
		void setText(QString text);

	protected:
		/**
		 * @brief Manage the touch events received by the item
		 * @param event Event received from scene
		 * @return true, if accepted; false otherwise
		 */
		bool sceneEvent(QEvent* event);
	public slots:

		/**
		 * @brief Sets the angle of the selector
		 * @param value
		 * @remarks Using this does not emit a onAngleChanged signal (should it?)
		 */
		void setAngle(int value);
	signals:
		/**
		 * @brief Emitted at the end of event once the angle is changes
		 */
		void signalAngleChanged();
	};
	class CustomArc;
	class PinJointLimitsSelector : public QObject, public QGraphicsItemGroup
	{
		Q_OBJECT
	private:
		cdJoint* p_physicsJoint;
		b2RevoluteJointDef* p_jointDef;	// shouldn't be needed
		RangeDialHandle* p_lowerLimitHandle;
		RangeDialHandle* p_upperLimitHandle;
		CustomArc* p_connectorItem;
		SelectableActions* p_enableLimits;
		SelectableActions* p_disableLimits;
		bool p_limitsEnabled;

		int lowerAngle;
		int upperAngle;
	public:
		PinJointLimitsSelector(cdPinJoint *physicsJoint, QGraphicsItem *graphicsparent);
		virtual ~PinJointLimitsSelector();

		int getLowerLimitAngle() const;
		int getUpperLimitAngle() const;

	public slots:
		void onAngleChanged();

	signals:
		void onLimitsChange(bool enableLimits, float lowerLimit, float upperLimit);
	};
}
#endif // PINJOINTLIMITSELECTOR_H
