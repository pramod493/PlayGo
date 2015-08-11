#pragma once
#include <QHash>
#include <QGraphicsObject>
#include <QGraphicsItemGroup>
#include <QGraphicsPixmapItem>
#include <QGesture>
#include "commonfunctions.h"
#include "cdi2qtwrapper.h"
#include "stroke.h"
#include "pixmap.h"
#include "polygon2d.h"
#include "physicsshape.h"
#include "box2dworld.h"
#include "QsLog.h"
#include <QPair>
#include <QGraphicsEffect>

namespace CDI
{
	class Page;
	class PhysicsJoint;
	/**
	 * @brief It is equivalent of Component in visualization size. This stores references to the all the children
	 * Also, note that all the children take care of their transform and therefore need not implement the
	 * boundingRect() function in this as well as component class.
	 */
	class Component : public QGraphicsObject/*QGraphicsItemGroup*/, protected QHash<QUuid, QGraphicsItem*>
	{
		Q_OBJECT
	public:
		enum { Type = UserType + COMPONENTVIEW };

		bool requiresRegeneration;	/**< Regenerate the physics info if mark is true */
		bool pendingPositionUpdate;	/**< Marks change in item position for a given iteration */
		float previousScale;		/**< Stores the previous scale. Allows for regenration only when scaled beyong a value */
		bool disableScaling;		/**< Disables the scaling on touch gestures */

		uint16 groupIndex;			/**< Box2D groupIndex for fixtures */
		uint16 maskBits;			/**< Box2D maskBits for fixtures */
		int16 categoryBits;			/**< Box2D categoryBits for fixtures */

	protected:
		QRectF itemBoundingRect;

		QUuid _id;

		b2Body* _physicsBody;

		QList<b2Fixture*> _fixtures;

		// Store the joint as well as its position w.r.t. unscaled component
		QList<PhysicsJoint*> _jointlist;

		QGraphicsPathItem* _anchorItem;

		QGraphicsPixmapItem* _lockScaleItem;

		float _density;

		bool _lockItem;

	public:
		Component(QGraphicsItem* parent = 0);

		virtual ~Component();

		/**
		 * @brief Bounding rectangle of the item in local transform
		 * @return Union of bounding rect of the children item
		 */
		QRectF boundingRect() const;

		/**
		 * @brief Recalculates the items bounding rect
		 */
		void recalculateBoundingRect();

		/**
		 * @brief Paint operations for the component. Draws the bounding rect when debugging
		 * @param painter QPainter for drawing
		 * @param option paint options
		 * @param widget QWidget parameter (Not sure what it does)
		 */
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

		/**
		 * @brief Returns Component's type. Useful for casting to right item type
		 * @return
		 */
		int type () const { return Type; }

		/**
		 * @brief Returns component's unique QUuid id
		 * @return id
		 */
		QUuid id() const { return _id; }

		/**
		 * @brief Returns b2Body pointer associated with the object
		 * @return
		 */
		b2Body* physicsBody() const { return _physicsBody; }

		void setPhysicsBody(b2Body* body);

		bool isStatic() const;

		void setStatic(bool value);

		bool isScalingDisabled() const;

		void setDisableScaling(bool value);

        void setHighlight(bool value);

        bool isHighlighted() const;

		bool isLocked() const;

		void lock(bool value);

		bool sceneEvent(QEvent *event);

		bool gestureEvent(QGestureEvent* gesture);

		bool touchEvent(QTouchEvent* event);

		using QHash<QUuid, QGraphicsItem*>::values;

		virtual void addToComponent(QGraphicsItem* itemToAdd);

		virtual void removeFromComponent(QGraphicsItem* itemToRemove);

		virtual void addJoint(PhysicsJoint* physicsJoint);

		virtual void removeJoint(PhysicsJoint* physicsJoint);

		virtual void removeJoint(b2Joint* joint);

		virtual void removeFromComponent(QUuid uid);

		virtual bool containsItem(QUuid uid);

		virtual QGraphicsItem* getItemById(QUuid uid);

		virtual void regenerateInternals();

		virtual QDataStream& serialize(QDataStream& stream) const;

		virtual QDataStream& deserialize(QDataStream& stream);

		float getDensity() const;

		void setDensity(float density);

		void onCollisionBitsUpdate();

	protected:
		virtual void addToHash(QUuid uid, QGraphicsItem* item);

		virtual void removeFromHash(QUuid id);

		/**
		 * @brief Adds the relevant fixture to physics object when a new
		 * child is added to the component
		 * @param graphicsitem
		 */
		virtual void addFixture(QGraphicsItem* graphicsitem);

		virtual bool isTouchEventAcceptable(QTouchEvent* event);

		virtual bool containsPoint(QPointF pos);
	signals:
		void onItemAdd(QGraphicsItem* item);

		void onShapeUpdate(Component* component);

		void onTransformChange(QGraphicsItem* component);

        void preComponentDelete(Component* component);

    public slots:
		void internalTransformChanged()
		{
			pendingPositionUpdate = true;
			emit onTransformChange(this);
		}
		friend class Page;
	};
}
