#include "component.h"
#include "stroke.h"
#include "pixmap.h"
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
#include "physicsjoint.h"
#include <QGraphicsTextItem>

namespace CDI
{
		Component::Component(QGraphicsItem *parent)
				: QGraphicsObject(parent)
		{
		_id = uniqueHash();

		_physicsBody = nullptr;		// Creation is done outside but deletion is performed internally
		_fixtures	= QList<b2Fixture*>();
		_jointlist = QList<cdJoint*>();

		// Disabling touch makes it hard to interact because one-touch pan is also disabled
		setAcceptTouchEvents(true);

		// Capture all the events sent to child items here
		setFiltersChildEvents(true);

		// TODO - Check if we really need this option set up. We can avoid this by
		// gettting rid of all event to the component so its not a big issue
		//setFlag(QGraphicsItem::ItemIsMovable);

		setZValue(Z_COMPONENTVIEW);

		// We can also use signals to find positionUpdates
		// component->update signal to Page->update signal to Physics Manager
		requiresRegeneration = false;
		pendingPositionUpdate = false;
		previousScale = 1.0f;

		disableScaling = false;

		//TODO - Think more on this part on how can dynamics be changed
		groupIndex		= -1;	// positive and same to enable collision
								// negative and same to disable collision
								// non-zero & different - use category and mask
								// zero - user category and mask
								// http://www.iforce2d.net/b2dtut/collision-filtering
		maskBits		= 0xFFFF;
		categoryBits	= 0x0000;

		componentFilter.groupIndex = groupIndex;
		componentFilter.maskBits = maskBits;
		componentFilter.categoryBits = categoryBits;

		_anchorItem		= nullptr;
		_lockScaleItem	= nullptr;

		_layerText = new QGraphicsSimpleTextItem(this);
		_layerText->setPos(0,0);
		_layerText->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		_layerText->setText(QString::number(groupIndex));

		_lockItem = false;

		_density = 1.0f;

		connect(this, SIGNAL(xChanged()),
				this, SLOT(internalTransformChanged()));

		connect(this, SIGNAL(yChanged()),
				this, SLOT(internalTransformChanged()));

		connect(this, SIGNAL(scaleChanged()),
				this, SLOT(internalTransformChanged()));

		connect(this, SIGNAL(rotationChanged()),
				this, SLOT(internalTransformChanged()));

#ifdef CDI_DEBUG_DRAW_SHAPE
		QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem(this);
		addToComponent(ellipse);
		ellipse->setRect(QRectF(-10,-10,20,20));
		ellipse->setTransform(QTransform());
#endif //CDI_DEBUG_DRAW_SHAPE
	}

	Component::Component(const Component &copy)
	{
		_id = uniqueHash();
		setParentItem(copy.parentItem());
		setParent(copy.parent());

		_physicsBody = NULL;

		setAcceptTouchEvents(copy.acceptTouchEvents());
		setFiltersChildEvents(true);
		setZValue(copy.zValue());

		disableScaling	= copy.disableScaling;
		previousScale	= copy.previousScale;
		componentFilter = copy.componentFilter;
		groupIndex		= copy.groupIndex;
		maskBits		= copy.maskBits;
		categoryBits	= copy.categoryBits;
		_density		= copy._density;

		////// Copy all the children
		auto listChildren = copy.childItems();
		for(QGraphicsItem* graphicsItem : listChildren)
		{
			switch(graphicsItem->type())
			{
			case Stroke::Type :
			{
				break;
			}
			case Polygon2D::Type :
			{
				Polygon2D* copyPolygon =
						qgraphicsitem_cast<Polygon2D*>(graphicsItem);
				Polygon2D* newPolygon = new Polygon2D(*copyPolygon);

				addToComponent(newPolygon);
				newPolygon->setTransform(copyPolygon->transform());
				break;
			}
			case Pixmap::Type :
			{
				Pixmap* copyPixmap =
						qgraphicsitem_cast<Pixmap*>(graphicsItem);
				Pixmap* newPixmap =
						new Pixmap(copyPixmap->pixmap(), copyPixmap->filename(), this);

				addToComponent(newPixmap);
				newPixmap->setTransform(copyPixmap->transform());
				break;
			}
			default:
				break;
			}
		}

		connect(this, SIGNAL(xChanged()),
				this, SLOT(internalTransformChanged()));

		connect(this, SIGNAL(yChanged()),
				this, SLOT(internalTransformChanged()));

		connect(this, SIGNAL(scaleChanged()),
				this, SLOT(internalTransformChanged()));

		connect(this, SIGNAL(rotationChanged()),
				this, SLOT(internalTransformChanged()));

		setTransform(copy.transform());
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
		Q_UNUSED(painter)
		// parent object does not need to manage children paint operations
#ifdef CDI_DEBUG_DRAW_SHAPE
		if (option->state & QStyle::State_Selected)
		{
			painter->setBrush(Qt::NoBrush);
			painter->drawRect(itemBoundingRect);
		}
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
		if (_physicsBody == nullptr) return;

		if (value)
		{
			_physicsBody->SetType(b2_staticBody);

			if (_anchorItem)
			{
				_anchorItem->show();
				return;
			}

			_anchorItem = new QGraphicsPathItem(this);
			_anchorItem->setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
			_anchorItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
			QPen pen = QPen(Qt::blue);
			pen.setWidth(3);
			_anchorItem->setPen(pen);

			QPainterPath path;
			float length = 20;
			path.moveTo(-length, -length);
			path.lineTo(length, length);
			path.moveTo(-length, length);
			path.lineTo(length, -length);

			_anchorItem->setPath(path);
			_anchorItem->setPos(boundingRect().center());
		} else
		{
			_physicsBody->SetType(b2_dynamicBody);
			if (_anchorItem)
				delete _anchorItem;
			_anchorItem = nullptr;
		}
	}

	bool Component::isScalingDisabled() const
	{
		return disableScaling;
	}

	void Component::setDisableScaling(bool value)
	{
		if (disableScaling == value) return;		// Nothing to do here

		disableScaling = value;

		if (value)
		{
			disableScaling = true;
			if (_lockScaleItem) delete _lockScaleItem;
			_lockScaleItem = new QGraphicsPixmapItem(this);
			_lockScaleItem->setPos(boundingRect().center() - QPointF(24,24));
			QPixmap tmpPixmap = QPixmap(":/images/overlay/lock.png");
			tmpPixmap = tmpPixmap.scaled(48,48, Qt::KeepAspectRatio);
			_lockScaleItem->setPixmap(tmpPixmap);
			_lockScaleItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		}
	}

	void Component::setHighlight(bool value)
	{
		if (!value)
		{
			if (graphicsEffect())
				graphicsEffect()->setEnabled(false);
			return;
		}

		if (graphicsEffect() == nullptr)
		{
			QGraphicsColorizeEffect* effect = new QGraphicsColorizeEffect(this);
			effect->setColor(QColor(255,100,100,255));
			effect->setStrength(0.75f);
			setGraphicsEffect(effect);
		} else
		{
			graphicsEffect()->setEnabled(true);
		}
	}

	bool Component::isHighlighted() const
		{
		if (graphicsEffect())
			return graphicsEffect()->isEnabled();
				return false;
		}

	bool Component::isLocked() const
	{
		return _lockItem;
	}

	void Component::lock(bool value)
	{
		if (_lockItem != value)
		{
			_lockItem = value;
		}
	}

	bool Component::sceneEvent(QEvent *event)
	{
		switch (event->type())
		{
		case QEvent::GraphicsSceneMousePress :
		case QEvent::GraphicsSceneMouseMove :
		case QEvent::GraphicsSceneMouseRelease :
		{
			event->accept();	// Do nothing?
			return true;
			break;
		}
		case QEvent::GraphicsSceneMouseDoubleClick :
		{
			setHighlight(!isHighlighted());
			event->accept();
			return true;
		}
		case QEvent::TouchBegin :
		case QEvent::TouchUpdate :
		case QEvent::TouchEnd :
		case QEvent::TouchCancel :
			event->accept();
			return touchEvent(static_cast<QTouchEvent*>(event));
			break;
		case QEvent::Gesture :	// ignore
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
		if (event->type()==QEvent::TouchBegin)
		{
			// Do not process when the touch event arrives
			if (isTouchEventAcceptable(event))
			{
				event->accept();
				return true;
			} else
			{
				return false;
			}
			// Marks the start of touch event to the object
			// Must accept this event to receive further events
		}

		if (event->touchPointStates() & Qt::TouchPointPressed)
		{
			qDebug() << "Extra touch point added" << "Ignoring..."
					 << event->touchPoints().count() << "fingers now";
			return false;
		}

		event->accept();

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

			if (!disableScaling)
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

	void Component::addJoint(cdJoint *physicsJoint)
	{
		// Do not add the item to the component because it can be added to one
		// item at any moment
		if (_jointlist.contains(physicsJoint) == false)
			_jointlist.push_back(physicsJoint);
	}

	void Component::removeJoint(cdJoint *physicsJoint)
	{
		if (_jointlist.contains(physicsJoint))
		{
			_jointlist.removeOne(physicsJoint);
			removeFromComponent(physicsJoint);
		}
	}

	void Component::removeJoint(b2Joint *joint)
	{
		for (auto internalJoint : _jointlist)
			if (internalJoint->joint() == joint)
			{
				removeJoint(internalJoint);
				return;
			}
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
			foreach (auto tmpJoint, _jointlist)
			{
				tmpJoint->updateJoint();
			}
		}
		// Reset the flags which requires regeneration
		requiresRegeneration = false;
		previousScale = scale();


	}

	QDataStream& Component::serialize(QDataStream& stream) const
	{
		float z_value = zValue();
		bool anchor_present = (_anchorItem == nullptr ? false : true);

		stream << _id;
		stream << _density;
		stream << disableScaling;
//		stream << groupIndex;
//		stream << maskBits;
//		stream << categoryBits;
		stream << transform();

		stream << _lockItem;

		// derived components

		stream << z_value;
		stream << anchor_present;


		return stream;
	}

	QDataStream& Component::deserialize(QDataStream& stream)
	{
		float z_value = 0;
		float anchor_present = false;

		stream >> _id;
		stream >> _density;
		stream >> disableScaling;
//		stream >> groupIndex;
//		stream >> maskBits;
//		stream >> categoryBits;
		QTransform t;
		stream >> t;
		setTransform(t);

		stream >> _lockItem;

		// derived components

		stream << z_value;
		stream >> anchor_present;

		setZValue(z_value);
		if (anchor_present)
		{}

		return stream;
	}

	float Component::getDensity() const
	{
		return _density;
	}

	void Component::setDensity(float density)
	{
		if (density < 0.00001f) {
			QLOG_ERROR() << "@Component::setDensity("<< density <<") is too less. Density unchanged.";
		}
		// TODO on density change
		_density = density;
		// Update all the components density
		for(auto fixture : _fixtures)
		{
			fixture->SetDensity(density);
		}
	}

	void Component::onCollisionBitsUpdate()
	{
		for(auto fixture : _fixtures)
		{
			fixture->SetFilterData(componentFilter);
		}
		if (_physicsBody)
			_physicsBody->SetAwake(true);
	}

	QList<QGraphicsItem*> Component::childItemByType(int itemType)
	{
		QList<QGraphicsItem*> allchilditems = childItems();
		QList<QGraphicsItem*> return_list;
		for(auto child : allchilditems)
			if (child->type() == itemType)
				return_list.push_back(child);
		return return_list;
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

		if (graphicsitem->isVisible() == false)	return;	// Hidden components do not contribute to the generation of components
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
			polygon->initializePhysicsShape();	// Lets go it again
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
					fixtureDef.density = _density;
					fixtureDef.filter = componentFilter;
					b2Fixture* fixture = _physicsBody->CreateFixture(&fixtureDef);
					_fixtures.push_back(fixture);
				}
			}
#ifdef CDI_DEBUG_DRAW_SHAPE
			image.save(f);
#endif //CDI_DEBUG_DRAW_SHAPE
		}
	}

	bool Component::isTouchEventAcceptable(QTouchEvent *event)
	{
		auto touchpoints = event->touchPoints();
		for (QList<QTouchEvent::TouchPoint>::const_iterator it = touchpoints.constBegin();
			 it != touchpoints.constEnd(); ++it)
		{
			const QTouchEvent::TouchPoint &tp = *(it);
			QPointF pos = tp.scenePos();
			if (containsPoint(pos))
			{
				return true;
			}
		}
		return false;
	}

	bool Component::containsPoint(QPointF pos)
	{
		QList<QGraphicsItem*> items = childItems();
		for(auto graphicsitem: items)
		{
			// NOTE - Apart from stroke, all the other object types are using
			// reimplementation of base class function and therefore need not
			// have a separate call
			switch (graphicsitem->type())
			{
			case Stroke::Type :
			{
				Stroke *stroke = qgraphicsitem_cast<Stroke*>(graphicsitem);
				if (stroke->contains(stroke->mapFromScene(pos), 25.0f))
					return true;
				break;
			}
			case Polygon2D::Type :
			{
				Polygon2D *polygon = qgraphicsitem_cast<Polygon2D*>(graphicsitem);
				if (polygon->containsPoint(polygon->mapFromScene(pos)))
					return true;
				break;
			}
			case QGraphicsPolygonItem::Type :
			{
				if (graphicsitem->contains(graphicsitem->mapFromScene(pos))) return true;
				break;
			}
			case Pixmap::Type :
			{
				Pixmap *pixmap = qgraphicsitem_cast<Pixmap*>(graphicsitem);
				if (pixmap->contains(pixmap->mapFromScene(pos)))
					return true;
				// TODO check if the point lies within one of the interior curves
				break;
			}
				// No other object allowed for selection.
			}
		}
		return false;
	}
}
