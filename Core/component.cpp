#include "component.h"
#include "stroke.h"
#include "pixmap.h"
#include "graphicssearchitem.h"
#include "polygon2d.h"
#include "assembly.h"
#include "physicsjoint.h"
#include "QsLog.h"
#include "physicsmanager.h"
#include <QImage>
#include <QMatrix4x4>
#include <QTransform>
#include <QGraphicsTransform>
#include <QMessageBox>
#include <QVector2D>
#include "physicsjoint.h"
namespace CDI
{
	Component::Component(QGraphicsItem *parent)
		: QGraphicsObject(parent)
	{
		_highlighted = false;
		_id = uniqueHash();

		_physicsBody = NULL;		// Creation is done outside but deletion is performed internally
		_fixtures	= QList<b2Fixture*>();
		_jointlist = QList<PhysicsJoint*>();

		setAcceptTouchEvents(true);

		/*grabGesture(Qt::TapGesture);
		grabGesture(Qt::TapAndHoldGesture);
		grabGesture(Qt::SwipeGesture);
		grabGesture(Qt::PanGesture);*/

		// TODO - Check if we really need this option set up. We can avoid this by
		// gettting rid of all event to the component so its not a big issue
        //setFlag(QGraphicsItem::ItemIsMovable);

		setZValue(Z_COMPONENTVIEW);

		// We can also use signals to find positionUpdates
		// component->update signal to Page->update signal to Physics Manager
		requiresRegeneration = false;
		pendingPositionUpdate = false;
		previousScale = 1.0f;

		connect(this, SIGNAL(xChanged()),
				this, SLOT(internalTransformChanged()));

		connect(this, SIGNAL(yChanged()),
				this, SLOT(internalTransformChanged()));

		connect(this, SIGNAL(scaleChanged()),
				this, SLOT(internalTransformChanged()));

		connect(this, SIGNAL(rotationChanged()),
				this, SLOT(internalTransformChanged()));

		_anchorItem = NULL;

#ifdef CDI_DEBUG_DRAW_SHAPE
		QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem(this);
		addToComponent(ellipse);
		ellipse->setRect(QRectF(-10,-10,20,20));
		ellipse->setTransform(QTransform());
#endif //CDI_DEBUG_DRAW_SHAPE
	}

	Component::~Component()
	{
		// Deletion handled by Page object
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
		Q_UNUSED(widget)
		Q_UNUSED(option)
		// parent object does not need to manage children paint operations
#ifdef CDI_DEBUG_DRAW_SHAPE
		if (option->state & QStyle::State_Selected)
		{
			painter->setBrush(Qt::NoBrush);
			painter->drawRect(itemBoundingRect);
		}
#endif //CDI_DEBUG_DRAW_SHAPE

#ifdef CDI_DEBUG_DRAW_SHAPE
		painter->setPen(QPen(Qt::blue));
		painter->setBrush(QBrush(Qt::NoBrush));
		painter->drawRect(itemBoundingRect);
#endif //CDI_DEBUG_DRAW_SHAPE
	}

	void Component::setPhysicsBody(b2Body *body)
	{
		if (body != _physicsBody)
		{
			if(_physicsBody)
				foreach(b2Fixture* fixture, _fixtures)
					_physicsBody->DestroyFixture(fixture);
			_physicsBody = body;
		}
	}

	bool Component::isStatic() const
	{
		if (_physicsBody)
		{
			if (_physicsBody->GetType() == b2_dynamicBody)
				return false;
			else return true;
		}
		return false;
	}

	void Component::setStatic(bool value)
	{
		if (_physicsBody)
		{
			if (value)
			{
				if (_anchorItem == NULL)
				{
					_anchorItem = new QGraphicsPathItem(this);
					_anchorItem->setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
					_anchorItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
					QPen pen = QPen(Qt::blue);
					pen.setWidth(2);
					_anchorItem->setPen(pen);

					QPainterPath path;
					float length = 15;
					path.moveTo(-length, -length);
					path.lineTo(length, length);
					path.moveTo(-length, length);
					path.lineTo(length, -length);
					_anchorItem->setPath(path);
				} else
				{
					_anchorItem->show();
				}
					_physicsBody->SetType(b2_staticBody);
			} else
			{
				_physicsBody->SetType(b2_dynamicBody);
				if (_anchorItem)
					delete _anchorItem;
				_anchorItem = NULL;
			}
		}
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
			// Single finger. Drag
			const QTouchEvent::TouchPoint &tp1 = event->touchPoints().first();
			if (QGraphicsItem::contains(mapFromScene(tp1.scenePos())) == false)
			{
				// TODO - This seems to not work with TUIO. Disbaling till further info is available
				//return false;
			}
			QPointF pan = tp1.scenePos() - tp1.lastScenePos();
			moveBy(pan.x(), pan.y());
			pendingPositionUpdate = true;
			emit onTransformChange(this);

		}
		if (event->touchPoints().count() == 2 &&
				(event->touchPointStates() & Qt::TouchPointMoved))
		{
			// Two finger
			const QTouchEvent::TouchPoint &tp1 = event->touchPoints()[0];
			const QTouchEvent::TouchPoint &tp2 = event->touchPoints()[1];
			QPointF tp1_itemPos = mapFromScene(tp1.scenePos());
			QPointF tp2_itemPos = mapFromScene(tp2.scenePos());

			/*if ((tp1.state() == Qt::TouchPointPressed) || (tp2.state() == Qt::TouchPointPressed))
			{
				// qDebug() << "At least one of points begins here. Ignore";
				return false;
			}*/

			if (!(QGraphicsItem::contains(tp1_itemPos) || QGraphicsItem::contains(tp2_itemPos)))
			{
				// Does not work properly with TUIO. Disabling till further diagnosis
				//return false;	// Touch point does not lie on the object. Do nothing
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
			pendingPositionUpdate = true;
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

		// Begin geometry change.
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
		// End geometry change

		/*******************************************************************************
		 * MUST UPDATE THE ITEM SHAPE BECAUSE THE TRANSFORM HAS UPDATED
		 * ****************************************************************************/

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
			pixmap->initializePhysicsShape();
			addToHash(pixmap->id(), pixmap);
			break;
		}
		case Polygon2D::Type :
		{
			Polygon2D* polygon = qgraphicsitem_cast<Polygon2D*>(item);
			polygon->initializePhysicsShape();
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
		addFixture(item);
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
			// Does not require regeneration of items
			break;
		}
		case Pixmap::Type :
		{
			Pixmap* pixmap = qgraphicsitem_cast<Pixmap*>(item);
			removeFromHash(pixmap->id());
			requiresRegeneration = true;
			break;
		}
		case Polygon2D::Type :
		{
			Polygon2D* polygon = qgraphicsitem_cast<Polygon2D*>(item);
			removeFromHash(polygon->id());
			requiresRegeneration = true;
			break;
		}
		case Component::Type :
		{
			Component* component = qgraphicsitem_cast<Component*>(item);
			removeFromHash(component->id());
			requiresRegeneration = true;
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

	void Component::addJoint(PhysicsJoint *physicsJoint)
	{
		if (_jointlist.contains(physicsJoint) == false)
			_jointlist.push_back(physicsJoint);
	}

	void Component::removeJoint(PhysicsJoint *physicsJoint)
	{
		if (_jointlist.contains(physicsJoint))
		_jointlist.removeOne(physicsJoint);
	}

	void Component::removeJoint(b2Joint *joint)
	{
		// TODO - Shouldn't we delete the joint as well?
		PhysicsJoint* physicsjoint = 0;
		foreach (PhysicsJoint* tmp, _jointlist)
			if (tmp->joint() == joint)
				physicsjoint = tmp;
		if (physicsjoint) _jointlist.removeOne(physicsjoint);
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

		if (_physicsBody != NULL)
		{
			foreach(b2Fixture* fixture, _fixtures)
				_physicsBody->DestroyFixture(fixture);	// Remove all fixtures
			_fixtures.clear();
			// Get children
			QList<QGraphicsItem*> itemList = childItems();
			foreach (QGraphicsItem* graphicsitem, itemList)
			{
				addFixture(graphicsitem);
			}

			// Update joint positions
			foreach (PhysicsJoint* tmpJoint, _jointlist)
			{
				tmpJoint->physicsManager()->updateJoint(tmpJoint);
			}
		}
		// Reset the flags which requires regeneration
		requiresRegeneration = false;
		previousScale = scale();


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

	void Component::addFixture(QGraphicsItem *graphicsitem)
	{
		// Note: this one is called internally and therefore does
		// not require regeneration. Regeneration is required on
		// removal of components form the component because we do not
		// b2Fixture and item mapping

		if (graphicsitem->isVisible() == false)  return;	// Hidden components do not contribute to the generation of components
		PhysicsShape* shape = 0;
		QSize r;
		if (graphicsitem->type() == Pixmap::Type)
		{
			Pixmap* pixmap = qgraphicsitem_cast<Pixmap*>(graphicsitem);
			shape = pixmap->physicsShape();
			r = pixmap->pixmap().size();
		}
		if (graphicsitem->type() == Polygon2D::Type)
		{
			Polygon2D* polygon = qgraphicsitem_cast<Polygon2D*>(graphicsitem);
			polygon->initializePhysicsShape();  // Lets go it again
			shape = polygon->physicsShape();
			r = polygon->boundingRect().toRect().size();
		}

		if (shape && _physicsBody)
		{
#ifdef CDI_DEBUG_DRAW_SHAPE
			QString f(QString("SaveShape")+uniqueHash().toString()+QString(".png"));
			//QRectF r = graphicsitem->boundingRect().toRect();
			QImage image = QImage(r.width(), r.height(),
								  QImage::Format_ARGB32_Premultiplied);
			image.fill(Qt::transparent);
			QPainter painter(&image);
			painter.setPen(QPen(Qt::black));
#endif // CDI_DEBUG_DRAW_SHAPE
			float mainScale = scale();

			float multiplier = getPhysicsScale() / mainScale;
			QTransform t = graphicsitem->transform().inverted();
			for (int i=0; i< shape->trias.size(); i++)
			{
				Triangle* tria = shape->trias[i];
				b2Vec2 vertices[3];
				for (int j=0; j < 3; j++) // Iterate over each vertex
				{
					Point2D newPos = t.map(tria->points[j]);
					vertices[j].Set(newPos.x() / multiplier, newPos.y() / multiplier);
				}
				int tempCount = 0;
				{
					// Not needed mostly
					int32 n = 3;
					b2Vec2 ps[b2_maxPolygonVertices];

					// Check if vertices don't overlap/ collinear
					for (int k=0; k< n; k++)
					{
						b2Vec2 v = vertices[k];

						bool unique = true;
						for (int l = 0; l < tempCount; ++l)
						{
							if (b2DistanceSquared(v, ps[l]) < 0.5f * b2_linearSlop)
							{
								unique = false;
								break;
							}
						}
						if (unique)
						{
							ps[tempCount++] = v;
						}
					}
				}
				if (tempCount >= 3)		// not enough unique vertices
				{
					#ifdef CDI_DEBUG_DRAW_SHAPE
						painter.drawLine(QPointF(vertices[0].x * multiplier, vertices[0].y * multiplier),
									QPointF(vertices[1].x * multiplier, vertices[1].y * multiplier));
						painter.drawLine(QPointF(vertices[2].x * multiplier, vertices[2].y * multiplier),
									QPointF(vertices[1].x * multiplier, vertices[1].y * multiplier));
						painter.drawLine(QPointF(vertices[2].x * multiplier, vertices[2].y * multiplier),
									QPointF(vertices[0].x * multiplier, vertices[0].y * multiplier));
					#endif // CDI_DEBUG_DRAW_SHAPE

					b2PolygonShape b2TriaShape;
					b2TriaShape.Set(vertices, 3);
					b2FixtureDef fixtureDef;
					fixtureDef.shape = &b2TriaShape;
					fixtureDef.density = 1.0f;
					fixtureDef.filter.groupIndex = -1;
					b2Fixture* fixture = _physicsBody->CreateFixture(&fixtureDef);
					_fixtures.push_back(fixture);
				}
			}
#ifdef CDI_DEBUG_DRAW_SHAPE
			image.save(f);
#endif //CDI_DEBUG_DRAW_SHAPE
		}
	}
}
