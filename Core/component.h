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

		bool requiresRegeneration;
		bool pendingPositionUpdate;
		float previousScale;

	protected:
		QRectF itemBoundingRect;

		bool _highlighted;
		QUuid _id;

		b2Body* _physicsBody;

		QList<b2Fixture*> _fixtures;

		// Store the joint as well as its position w.r.t. unscaled component
		QList<PhysicsJoint*> _jointlist;

		QGraphicsPathItem* _anchorItem;

	public:
		Component(QGraphicsItem* parent = 0);

		virtual ~Component();

		QRectF boundingRect() const;

		void recalculateBoundingRect();

		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

		int type () const { return Type; }

		QUuid id() const { return _id; }

		b2Body* physicsBody() const { return _physicsBody; }

		void setPhysicsBody(b2Body* body);

		bool isStatic() const;

		void setStatic(bool value);

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

	protected:
		virtual void addToHash(QUuid uid, QGraphicsItem* item);

		virtual void removeFromHash(QUuid id);

		/**
		 * @brief Adds the relevant fixture to physics object when a new
		 * child is added to the component
		 * @param graphicsitem
		 */
		virtual void addFixture(QGraphicsItem* graphicsitem);

	signals:
		void onItemAdd(QGraphicsItem* item);

		void onShapeUpdate(Component* component);

		void onTransformChange(QGraphicsItem* component);

    public slots:
		void internalTransformChanged()
		{
			pendingPositionUpdate = true;
			emit onTransformChange(this);
		}

		friend class Page;
	};
}
