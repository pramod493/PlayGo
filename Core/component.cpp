#include "component.h"
#include "stroke.h"
#include "pixmap.h"
#include "graphicssearchitem.h"
#include "polygon2d.h"
#include "assembly.h"
#include "QsLog.h"
#include <QMatrix4x4>
#include <QTransform>
#include <QGraphicsTransform>
#include <QMessageBox>
#include <QVector2D>
namespace CDI
{
	Component::Component(QGraphicsItem *parent)
		: QGraphicsObject(parent)
	{
		_highlighted = false;
		_id = uniqueHash();

		_physicsBody = NULL;

		setAcceptTouchEvents(true);
		//setFlag(QGraphicsItem::ItemIsMovable);

		// We can also use signals to find positionUpdates
		// component->update signal to Page->update signal to Physics Manager
		pendingPositionUpdate = false;
		pendingScalingUpdate = false;
		previousScale = 1.0f;


		//		setFlag(QGraphicsItem::ItemIgnoresTransformations,true);
		//		grabGesture(Qt::PinchGesture);
		//		grabGesture(Qt::SwipeGesture);
		//		grabGesture(Qt::PanGesture);

		connect(this, SIGNAL(xChanged()),
				this, SLOT(internalTransformChanged()));

		connect(this, SIGNAL(yChanged()),
				this, SLOT(internalTransformChanged()));

		connect(this, SIGNAL(rotationChanged()),
				this, SLOT(internalTransformChanged()));
	}

	Component::~Component()
	{
		// Deletes the children as well
		if (parentItem()!= NULL)
		{
			QGraphicsItem* item = parentItem();
			if (item->type() == Assembly::Type)
			{
				Assembly* parentAssembly = qgraphicsitem_cast<Assembly*>(item);
				// On item delete, destroy reference from assembly.
				parentAssembly->removeComponent(this);
			}
		}

		QList<QGraphicsItem*> children = childItems();
		for (int i=0; i < children.size(); i++)
			delete children[i];

		if (_physicsBody != NULL)
		{
			b2World* world = _physicsBody->GetWorld();
			world->DestroyBody(_physicsBody);
		}
	}

	QRectF Component::boundingRect() const
	{
		return itemBoundingRect;
	}

	void Component::recalculateBoundingRect()
	{
		itemBoundingRect = childrenBoundingRect();
	}

	void Component::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
	{
		Q_UNUSED(widget);
		// parent object does not need to manage children paint operations
		if (option->state & QStyle::State_Selected) {
			painter->setBrush(Qt::NoBrush);
			painter->drawRect(itemBoundingRect);
		}

#ifdef CDI_DEBUG_DRAW_SHAPE
		painter->setBrush(Qt::NoBrush);
		painter->drawRect(itemBoundingRect);
#endif //CDI_DEBUG_DRAW_SHAPE
	}

	bool Component::sceneEvent(QEvent *event)
	{
		switch (event->type())
		{
		case QEvent::TouchBegin :
		case QEvent::TouchUpdate :
		case QEvent::TouchEnd :
		case QEvent::TouchCancel :
			event->accept();
			return touchEvent(static_cast<QTouchEvent*>(event));
			break;
		case QEvent::Gesture :
			//			qDebug() << "Gesture received component";
			//			gestureEvent(static_cast<QGestureEvent*>(event));
			//			event->accept();
			//			return true;
			break;
		}
		return QGraphicsObject::sceneEvent(event);
	}

	bool Component::gestureEvent(QGestureEvent *gesture)
	{
		if (QGesture *swipe = gesture->gesture(Qt::SwipeGesture))
		{

		} else if (QGesture *pan = gesture->gesture(Qt::PanGesture))
		{
			QPanGesture* panGesture = static_cast<QPanGesture*>(pan);
			setPos(pos() + panGesture->offset() - panGesture->lastOffset());
		} else if (QGesture *pinch = gesture->gesture(Qt::PinchGesture))
		{
			float rotationDelta = 0;
			float currentStepScaleFactor = 1;
			QPinchGesture* pinchGesture = static_cast<QPinchGesture*>(pinch);
			QPinchGesture::ChangeFlags changeFlags = pinchGesture->changeFlags();
			setTransformOriginPoint(pinchGesture->centerPoint());
			setPos(pos() + pinchGesture->centerPoint() - pinchGesture->lastCenterPoint());
			if (changeFlags & QPinchGesture::RotationAngleChanged) {
				rotationDelta = pinchGesture->rotationAngle() - pinchGesture->lastRotationAngle();
				setRotation(rotationDelta + rotation());
			}
			if (changeFlags & QPinchGesture::ScaleFactorChanged) {
				currentStepScaleFactor = scale() * pinchGesture->totalScaleFactor();
				// Disable for extremely small and large objects
				if (currentStepScaleFactor > 0.25f && currentStepScaleFactor < 5)
					setScale(currentStepScaleFactor * scale());
			}
			if (pinchGesture->state() == Qt::GestureFinished) {
				//					scaleFactor *= currentStepScaleFactor;
				currentStepScaleFactor = 1;
			}
		}
		return true;
	}

	bool Component::touchEvent(QTouchEvent *event)
	{
		// Do not process when the touch event arrives
		if (event->type()==QEvent::TouchBegin)
		{
			// Marks the start of touch event to the object
			// Must accept this event to receive further events
			return true;
		}
		if (event->touchPointStates() & Qt::TouchPointPressed)
		{
			return false;
		}
		//		if (event->type() == QEvent::TouchEnd || event->type() == QEvent::TouchCancel)
		//		{
		//			setTransformOriginPoint(0,0);
		//			return true;
		//		}

		//		if ((event->touchPointStates() & Qt::TouchPointPressed) ||		// Start of input
		//			(event->touchPointStates() & Qt::TouchPointReleased))		// Touch leaving
		//		{
		//			// Ignore begninning/end of touch
		//			return false;	// Nothing to do if point does not move
		//		}

		if (event->touchPoints().count() == 1 &&
				(event->touchPointStates() & Qt::TouchPointMoved))
		{
			qDebug() << "Pan";
			// Single finger. Drag
			const QTouchEvent::TouchPoint &tp1 = event->touchPoints().first();

			if (QGraphicsItem::contains(mapFromScene(tp1.scenePos())) == false)
				return false;	// Point outside object

			QPointF pan = tp1.scenePos() - tp1.lastScenePos();
			moveBy(pan.x(), pan.y());

			emit onTransformChange(this);

		}
		if (event->touchPoints().count() == 2)
			qDebug() << "2 touch points";
		if (event->touchPoints().count() == 2 &&
				(event->touchPointStates() & Qt::TouchPointMoved))
		{
			// Two finger
			const QTouchEvent::TouchPoint &tp1 = event->touchPoints()[0];
			const QTouchEvent::TouchPoint &tp2 = event->touchPoints()[1];
			QPointF tp1_itemPos = mapFromScene(tp1.scenePos());
			QPointF tp2_itemPos = mapFromScene(tp2.scenePos());

			//			if ((tp1.state() == Qt::TouchPointPressed) || (tp2.state() == Qt::TouchPointPressed))
			//			{
			//				// qDebug() << "At least one of points begins here. Ignore";
			//				return false;
			//			}

			if (!(QGraphicsItem::contains(tp1_itemPos) || QGraphicsItem::contains(tp2_itemPos)))
			{
				qDebug() << "Touch point lies outside shape";
				return false;	// Touch point does not lie on the object. Do nothing
			}

			QPointF tp1_lastItemPos = mapFromScene(tp1.lastScenePos());
			QPointF tp2_lastItemPos = mapFromScene(tp2.lastScenePos());
			QPointF pan = QPointF(); float rot = 0; float scaleMul = 1;
			// // Keep pan as single finger event only
			{
				pan = ( tp1.scenePos() + tp2.scenePos()
						- tp1.lastScenePos() - tp2.lastScenePos()
						) / 2.0f;
				moveBy(pan.x(), pan.y());
			}
			QPointF tmpOrigin =(tp1.scenePos() + tp1.lastScenePos())/2.0f;
			setTransformOriginPoint(mapFromScene(tmpOrigin));
			{
				float prevmag = euclideanDistance(&tp2_lastItemPos, &tp1_lastItemPos);
				float currentmag = euclideanDistance(&tp2_itemPos, &tp1_itemPos);
				float currentScale = scale();
				scaleMul = currentScale* currentmag / prevmag;
				if (scaleMul > 0.25f && scaleMul < 5.0f)
					setScale(scaleMul);
			}
			{
				// Offset previous event points
				tp1_lastItemPos = tp1_lastItemPos + pan;
				tp2_lastItemPos = tp2_lastItemPos + pan;
				QVector2D v1 = QVector2D(tp1_itemPos - tp2_itemPos);
				QVector2D v2 = QVector2D(tp1_lastItemPos - tp2_lastItemPos);
				rot = atan2(v1.y(), v1.x()) - atan2(v2.y(), v2.x());
				rot = rotation() + (rot * 180.0f) / _PI_;
				setRotation(rot);
			}
			setTransformOriginPoint(QPointF(0,0));
			emit onTransformChange(this);
		}
		return true;
	}

	void Component::addToComponent(QGraphicsItem *item)
	{
		// Do not use the object's add function
		if (!item) {
			QLOG_INFO() << "cannot add null item";
			return;
		}
		if (item == this) {
			QLOG_WARN() << "cannot add a group to itself";
			return;
		}

		if (item->parentItem())
		{
			QGraphicsItem* parent = item->parentItem();
			if (parent->type() == Component::Type)
			{
				Component* parentComponent = qgraphicsitem_cast<Component*>(parent);
				parentComponent->removeFromComponent(item);
			}
		}

		bool ok;
		QTransform itemTransform = item->itemTransform(this, &ok);

		if (!ok) {
			QLOG_WARN() << "could not find a valid transformation from item to group coordinates";
			return;
		}

		QTransform newItemTransform(itemTransform);
		item->setPos(mapFromItem(item, 0, 0));
		item->setParentItem(this);

		// removing position from translation component of the new transform
		if (!item->pos().isNull())
			newItemTransform *= QTransform::fromTranslate(-item->x(), -item->y());

		// removing additional transformations properties applied with itemTransform()
		QPointF origin = item->transformOriginPoint();
		QMatrix4x4 m;
		QList<QGraphicsTransform*> transformList = item->transformations();
		for (int i = 0; i < transformList.size(); ++i)
			transformList.at(i)->applyTo(&m);
		newItemTransform *= m.toTransform().inverted();
		newItemTransform.translate(origin.x(), origin.y());
		newItemTransform.rotate(-item->rotation());
		newItemTransform.scale(1/item->scale(), 1/item->scale());
		newItemTransform.translate(-origin.x(), -origin.y());

		// ### Expensive, we could maybe use dirtySceneTransform bit for optimization

		item->setTransform(newItemTransform);

		itemBoundingRect |= itemTransform.mapRect(item->boundingRect() | item->childrenBoundingRect());

		prepareGeometryChange();

		switch (item->type())
		{
		case Stroke::Type :
		{
			Stroke* stroke = qgraphicsitem_cast<Stroke*>(item);
			addToHash(stroke->id(), stroke);
			break;
		}
		case Pixmap::Type :
		{
			Pixmap* pixmap = qgraphicsitem_cast<Pixmap*>(item);
			addToHash(pixmap->id(), pixmap);
			break;
		}
		case Polygon2D::Type :
		{
			Polygon2D* polygon = qgraphicsitem_cast<Polygon2D*>(item);
			addToHash(polygon->id(), polygon);
			break;
		}
		case Component::Type :
		{
			Component* component = qgraphicsitem_cast<Component*>(item);
			addToHash(component->id(), component);
			break;
		}
		}

		update();
	}

	void Component::removeFromComponent(QGraphicsItem *item)
	{
		if (!item) {
			QLOG_WARN() << "QGraphicsItemGroup::removeFromGroup: cannot remove null item";
			return;
		}

		switch (item->type())
		{
		case Stroke::Type :
		{
			Stroke* stroke = qgraphicsitem_cast<Stroke*>(item);
			removeFromHash(stroke->id());
			break;
		}
		case Pixmap::Type :
		{
			Pixmap* pixmap = qgraphicsitem_cast<Pixmap*>(item);
			removeFromHash(pixmap->id());
			break;
		}
		case Polygon2D::Type :
		{
			Polygon2D* polygon = qgraphicsitem_cast<Polygon2D*>(item);
			removeFromHash(polygon->id());
			break;
		}
		case Component::Type :
		{
			Component* component = qgraphicsitem_cast<Component*>(item);
			removeFromHash(component->id());
			break;
		}
		}

		QGraphicsItem *newParent = parentItem();

		// COMBINE
		bool ok;
		QTransform itemTransform;
		if (newParent)
			itemTransform = item->itemTransform(newParent, &ok);
		else
			itemTransform = item->sceneTransform();

		QPointF oldPos = item->mapToItem(newParent, 0, 0);
		item->setParentItem(newParent);
		item->setPos(oldPos);

		// removing position from translation component of the new transform
		if (!item->pos().isNull())
			itemTransform *= QTransform::fromTranslate(-item->x(), -item->y());

		// removing additional transformations properties applied
		// with itemTransform() or sceneTransform()
		QPointF origin = item->transformOriginPoint();
		QMatrix4x4 m;
		QList<QGraphicsTransform*> transformList = item->transformations();
		for (int i = 0; i < transformList.size(); ++i)
			transformList.at(i)->applyTo(&m);
		itemTransform *= m.toTransform().inverted();
		itemTransform.translate(origin.x(), origin.y());
		itemTransform.rotate(-item->rotation());
		itemTransform.scale(1 / item->scale(), 1 / item->scale());
		itemTransform.translate(-origin.x(), -origin.y());

		// ### Expensive, we could maybe use dirtySceneTransform bit for optimization

		item->setTransform(itemTransform);
		// ### Quite expensive. But removeFromGroup() isn't called very often.
		prepareGeometryChange();

		// Component object in itself does not contain any renderable item and therefore we need to pick only the children
		itemBoundingRect = childrenBoundingRect();

	}

	void Component::removeFromComponent(QUuid uid)
	{
		if (QHash<QUuid, QGraphicsItem*>::contains(uid))
		{
			QGraphicsItem* item = QHash<QUuid, QGraphicsItem*>::value(uid);
			removeFromComponent(item);
		}
	}

	bool Component::containsItem(QUuid uid)
	{
		if (QHash<QUuid, QGraphicsItem*>::contains(uid))
			return true;
		return false;
	}

	QGraphicsItem* Component::getItemById(QUuid uid)
	{
		if (QHash<QUuid, QGraphicsItem*>::contains(uid))
			return QHash<QUuid, QGraphicsItem*>::value(uid);
		return NULL;
	}

	void Component::regenerateInternals()
	{
		// Not sure what to do but mostly will need to update physics shape
		if (_physicsBody != NULL)
		{
			// Get children
			QList<QGraphicsItem*> itemList = childItems();
			foreach (QGraphicsItem* graphicsitem, itemList)
			{
				if (graphicsitem->type() == Pixmap::Type)
				{
					Pixmap* pixmap = qgraphicsitem_cast<Pixmap*>(graphicsitem);
					PhysicsShape* physicsShape = pixmap->physicsShape();
					//					QTransform relativeTransform = pixmap->itemTransform()
				}
			}
		}
	}

	QDataStream& Component::serialize(QDataStream& stream) const
	{
		stream << _id;
		return stream;
	}

	QDataStream& Component::deserialize(QDataStream& stream)
	{
		stream >> _id;
		return stream;
	}

	void Component::addToHash(QUuid uid, QGraphicsItem* item)
	{
		if (QHash<QUuid, QGraphicsItem*>::contains(uid)) return;
		insert(uid, item);
	}

	void Component::removeFromHash(QUuid uid)
	{
		if (QHash<QUuid, QGraphicsItem*>::contains(uid)) remove(uid);
	}
}
