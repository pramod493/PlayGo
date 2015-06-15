#include "component.h"
#include "page.h"
#include "polygon2d.h"
#include "stroke.h"
#include "image.h"

namespace CDI
{
	Component::Component(Page *parent)
		//: AbstractModelItem()
	{
		mask = 0;
		_id = uniqueHash();
		_pagePtr = parent;
		_assemblyParent  = NULL;
		_transform = QTransform();
		_inverseTransform = QTransform();

		_position = Point2D(0,0);
		_scale = 1;
		_rotation = 0.0f;

		_transformModified = false;

	}

	Component::~Component()
	{
		// Do not call directly. Rather call from Page::destroyComponent()
		// delete all the contained components;
		QList<AbstractModelItem*> allitems = values();
		foreach(AbstractModelItem* item, allitems)
			delete item;
	}

	QUuid Component::id() const
	{
		return _id;
	}

	Assembly* Component::parentAssembly()
	{
		return _assemblyParent;
	}

	void Component::setParentAssembly(Assembly *assembly)
	{
		_assemblyParent = assembly;
	}

	ItemType Component::type() const
	{
		return ItemType::COMPONENT;
	}

	Point2D Component::position()
	{
		return _position;
	}

	float Component::scale()
	{
		return _scale;
	}

	float Component::rotation()
	{
		return _rotation;
	}

	void Component::setPosition(float x, float y)
	{
		_position.setX(x);
		_position.setY(y);

		_transformModified = true;
	}

	void Component::translateBy(float x, float y)
	{
		_position += Point2D(x, y);

		_transformModified = true;
	}

	void Component::translateTo(float x, float y)
	{
		translateBy(x - _position.x(), y - _position.y());
	}

	void Component::setRotation(float rot)
	{
		_rotation = rot;

		_transformModified = true;
	}

	void Component::rotateBy(float deltaRot)
	{
		_rotation += deltaRot;

		_transformModified = true;
	}

	void Component::scaleBy(float scaleFactor)
	{
		_scale *= scaleFactor;
	}

	void Component::onIdUpdate(QUuid oldID, QUuid newID)
	{
		if (contains(oldID))
		{
			AbstractModelItem* item = value(oldID);
			remove(oldID);
			insert(newID, item);
		}
	}

	QTransform Component::transform() const
	{
		return _transform;
	}

	QTransform Component::globalTransform() const
	{
		return transform();
	}

	QTransform Component::itemTransform(AbstractModelItem* item) const
	{
		return transform() * (item->transform());
	}

	QTransform Component::itemTransform(QUuid itemId) const
	{
		QTransform t = transform();
		if (contains(itemId))
		{
			AbstractModelItem* item = value(itemId);
			t = item->transform();
		}
		return t;
	}

	void Component::setTransform(QTransform& transform, bool combine)
	{
		mask |= isTransformed;
		_transform = (combine ? _transform * transform : transform);
		_inverseTransform = _transform.inverted();

		_transformModified = true;

		qDebug() << "@Component::setTransform()";
		if (_pagePtr != NULL) _pagePtr->onComponentUpdate(this);
	}

	Stroke* Component::addStroke(QColor color, float thickness)
	{
		Stroke* stroke = new Stroke(this, color, thickness);
		addItem(stroke);
		return stroke;
	}

	Stroke* Component::addStroke(const QVector<Point2DPT> &points, QColor color, float thickness)
	{
		Stroke* stroke = new Stroke(this, points, color, thickness);
		addItem(stroke);
		return stroke;
	}

	Image* Component::addImage()
	{
		Image* image = new Image(this);
		addItem(image);
		return image;
	}

	Image* Component::addImage(const QString filename)
	{
		Image* image = new Image(this, filename);
		addItem(image);
		return image;
	}

	Image* Component::addImage(const QPixmap &pixmap, QString filename)
	{
		Image* image = new Image(this, pixmap, filename);
		addItem(image);
		return image;
	}

	void Component::addItem(AbstractModelItem* item)
	{
		if (contains(item->id()) == false)
			insert(item->id(), item);
	}

	bool Component::removeItem(AbstractModelItem* item)
	{
		return removeItem(item->id());
	}

	bool Component::removeItem(QUuid itemId)
	{
		bool retval = (remove(itemId) != 0 ? true : false);
		return retval;
	}

	bool Component::containsItem(AbstractModelItem *key, bool searchRecursive)
	{
		return containsItem(key->id(), searchRecursive);
	}

	bool Component::containsItem(QUuid key, bool searchRecurive)
	{
		if (contains(key)) return true;

		/*if (searchRecurive)
		{
			// Search into member components if they are of relevant type
			QHash<QUuid, AbstractModelItem*>::const_iterator iter;
			for (iter = constBegin(); iter != constEnd(); ++iter)
			{
				AbstractModelItem* item = iter.value();
				if (item->type()== ItemType::COMPONENT)
				{
					Component* component = static_cast<Component*>(item);
					if (component->containsItem(key)) return true;
				}
			}
		}*/
		return false;
	}

	bool Component::containsItem(QString key, bool searchRecursive)
	{
		QUuid id = QUuid(key);
		if (id.isNull()) return false;	// INVALID ID
		return containsItem(id, searchRecursive);
	}

	AbstractModelItem* Component::getItemPtrById(QUuid id/*, bool searchRecursive*/)
	{
		if (contains(id))
			return value(id);
		return NULL;
		/*
		if (!searchRecursive)
		{
			if (contains(id)) return value(id);
			return NULL;
		}
		// Iterate over all the items in hash and check if any of them is component
		QHash<QUuid, AbstractModelItem*>::const_iterator iter;
		for (iter = constBegin(); iter != constEnd(); ++iter)
		{
			AbstractModelItem* item = iter.value();
			if (item->type()== ItemType::COMPONENT)
			{
				Component* component = static_cast<Component*>(item);
				if (component->containsItem(id, searchRecursive))
				{
					return component->getItemPtrById(id);
				}
			}
		}
		return NULL;*/
	}

	QDataStream& Component::serialize(QDataStream& stream) const
	{
		stream << size();
		if (!isEmpty())
		{
			// Just serialize the items for now
			QHash<QUuid, AbstractModelItem*>::const_iterator iter;
			for (iter = constBegin(); iter != constEnd(); ++iter)
			{
				AbstractModelItem* item = iter.value();
				int j = item->type();
				stream << j;
				stream << *item;
			}
		}
		return stream;
	}

	QDataStream& Component::deserialize(QDataStream& stream)
	{
		int num_items;
		stream >> num_items;
		if (num_items!= 0)
		{
			for (int i=0; i < num_items; i++)
			{
				int j;
				stream >> j;
				AbstractModelItem* item = createEmptyItem(getItemType(j));
				stream >> *item;
				addItem(item);
			}
		}
		return stream;
	}

	AbstractModelItem* Component::createEmptyItem(ItemType itemType)
	{
		AbstractModelItem* ptr = NULL;
		switch (itemType)
		{
		case ItemType::STROKE :
			ptr = addStroke();
			break;
		case ItemType::IMAGE :
			qDebug() << "Creating empty Image object";
			ptr = addImage();
			break;
		case ItemType::POLYGON2D :
			ptr = new Polygon2D(this);
			addItem(ptr);
			break;
		case ItemType::COMPONENT :
			qDebug() << "Components are not supposed to contain components"
					 << "@Component::createEmptyItem(ItemType): AbstractModelItem*";
			break;
		default :
			qDebug() << "@Component::createEmptyItem(itemType: ItemType)"
					 << "Default constructor for given itemtype is not available. Returning NULL";
		}
		return ptr;
	}

	void Component::updateTransform()
	{
		_transform = QTransform();
		_transform = _transform.translate(_position.x(), _position.y());
		_transform = _transform.scale(_scale, _scale);
		_transform = _transform.rotate(_rotation);

		_inverseTransform = _transform.inverted();
	}

	void Component::onItemDelete(AbstractModelItem* item)
	{
		onItemDelete(item->id());
	}

	void Component::onItemDelete(QUuid itemKey)
	{
		if (contains(itemKey))
		{
			remove(itemKey);
			if (_pagePtr != NULL) _pagePtr->onItemRemove(itemKey);
		}
	}

	void Component::onItemUpdate(AbstractModelItem* item)
	{
		if (contains(item->id()) && _pagePtr != NULL)
			_pagePtr->onItemUpdate(item->id());
	}

	void Component::onItemUpdate(QUuid itemKey)
	{
		if (contains(itemKey) && _pagePtr != NULL)
			_pagePtr->onItemUpdate(itemKey);
	}
}
