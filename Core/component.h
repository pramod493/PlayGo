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

#include <QDebug>

namespace CDI
{
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

	protected:
		QRectF itemBoundingRect;

		bool _highlighted;
		QUuid _id;

		b2Body* _physicsBody;

    public:
		Component(QGraphicsItem* parent = 0);

		virtual ~Component();

		QRectF boundingRect() const;

		void recalculateBoundingRect();

		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

		int type () const { return Type; }

		QUuid id() const { return _id; }

		bool sceneEvent(QEvent *event);

		bool gestureEvent(QGestureEvent* gesture);

		bool touchEvent(QTouchEvent* event);

		using QHash<QUuid, QGraphicsItem*>::values;

		virtual void addToComponent(QGraphicsItem* itemToAdd);

		virtual void removeFromComponent(QGraphicsItem* itemToRemove);

		virtual void removeFromComponent(QUuid uid);

		virtual bool containsItem(QUuid uid);

		virtual QGraphicsItem* getItemById(QUuid uid);

		virtual void regenerateInternals();

		virtual QDataStream& serialize(QDataStream& stream) const;

		virtual QDataStream& deserialize(QDataStream& stream);

	protected:
		virtual void addToHash(QUuid uid, QGraphicsItem* item);

		virtual void removeFromHash(QUuid id);
    };
}
