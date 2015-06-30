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
		QHash<QUuid, AbstractModelItem*>::const_iterator iter;
		for (iter = constBegin(); iter != constEnd(); ++iter)
		{
			AbstractModelItem* item = iter.value();
			delete item;
		}
		clear();
		if (_pagePtr != NULL)
		{
			_pagePtr->destroyComponent(this);
		}
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
		return COMPONENT;
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

		_transformModified = true;
		_componentScaled = true;
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

	QTransform Component::transform()
	{
		if (_transformModified || _componentScaled)
		{
			_transform = QTransform();
			/*_transform = */_transform.rotate(_rotation);
			/*_transform = */_transform.scale(_scale, _scale);
			/*_transform = */_transform.translate
					(_position.x(), _position.y());
			_transformModified = false;
			_componentScaled = false;
		}
		return _transform;
	}

	QTransform Component::globalTransform()
	{
		return transform();
	}

	QTransform Component::itemTransform(AbstractModelItem* item)
	{
		return transform() * (item->transform());
	}

	QTransform Component::itemTransform(QUuid itemId)
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
		// NOTE This does not check for difference inscaling along different axes as well as
		// for
		{
			_transform = (combine ? _transform * transform : transform);
			float *tmpRotation, *tmpScale;
			tmpRotation = new float;
			tmpScale = new float;
			Point2D* tmpTranslation = new Point2D();
			extractTransformComponents(_transform, tmpRotation, tmpScale, tmpTranslation);

			_rotation = *tmpRotation;
			_scale = *tmpScale;
			_position = *tmpTranslation;

			delete tmpRotation;
			delete tmpScale;
			delete tmpTranslation;
		}

		_inverseTransform = _transform.inverted();

		if (_pagePtr != NULL) _pagePtr->onItemUpdate(id());
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
		{
			insert(item->id(), item);
		}
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

		/* NOTE - This will be useful in searching for items contained within
		 * AbstractModelItem objects such as PhysicsShape and PhysicsJoint
		 * if (searchRecurive)
		{
			// Search into member components if they are of relevant type
			QHash<QUuid, AbstractModelItem*>::const_iterator iter;
			for (iter = constBegin(); iter != constEnd(); ++iter)
			{
				AbstractModelItem* item = iter.value();
				if (item->type()== COMPONENT)
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
	}

	QDataStream& Component::serialize(QDataStream& stream) const
	{
		stream << _transform;
		stream << _inverseTransform;
		stream << _position;
		stream << _scale;
		stream << _rotation;

		stream << size();
		if (!isEmpty())
		{
			// Just serialize the items for now
			QHash<QUuid, AbstractModelItem*>::const_iterator iter;
			for (iter = constBegin(); iter != constEnd(); ++iter)
			{
				AbstractModelItem* item = iter.value();
				int j = item->type();
				// Do not write items which are not supported by the component
				if (isItemTypeSupported(item->type()))
				{
					stream << j;
					item->serialize(stream);
				}
			}
		}
		return stream;
	}

	QDataStream& Component::deserialize(QDataStream& stream)
	{
		stream >> _transform;
		stream >> _inverseTransform;
		stream >> _position;
		stream >> _scale;
		stream >> _rotation;

		int num_items;
		stream >> num_items;
		if (num_items!= 0)
		{
			for (int i=0; i < num_items; i++)
			{
				int j;
				stream >> j;
				ItemType t = getItemType(j);
				if (isItemTypeSupported(t))
				{
					AbstractModelItem* item = createEmptyItem(getItemType(j));
					item->deserialize(stream);
					addItem(item);
				}
			}
		}
		return stream;
	}

	AbstractModelItem* Component::createEmptyItem(ItemType itemType)
	{
		AbstractModelItem* ptr = NULL;
		switch (itemType)
		{
		case STROKE :
			ptr = addStroke();
			break;
		case IMAGE :
			qDebug() << "Creating empty Image object";
			ptr = addImage();
			break;
		case POLYGON2D :
			ptr = new Polygon2D(this);
			addItem(ptr);
			break;
		case COMPONENT :
			qDebug() << "Components are not supposed to contain components"
					 << "@Component::createEmptyItem(ItemType): AbstractModelItem*";
			break;
		default :
			qDebug() << "@Component::createEmptyItem(itemType: ItemType)"
					 << "Default constructor for given itemtype is not available. Returning NULL";
		}
		return ptr;
	}

	bool Component::isItemTypeSupported(ItemType t) const
	{
		switch (t)
        {
        case STROKE :
        case IMAGE :
        case POLYGON2D :
            return true;
        default:
            return false;
        }
        return false;
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
		onItemUpdate(item->id());
	}

	void Component::onItemUpdate(QUuid itemKey)
	{
		if (contains(itemKey) && _pagePtr != NULL)
			_pagePtr->onItemUpdate(itemKey);
	}

	void Component::onItemDisplayUpdate(AbstractModelItem *item)
	{
		onItemDisplayUpdate(item->id());
	}

	void Component::onItemDisplayUpdate(QUuid itemKey)
	{
		if(contains(itemKey) && _pagePtr != NULL)
			_pagePtr->onItemDisplayUpdate(itemKey);
	}

	void Component::onItemTransformUpdate(AbstractModelItem *item)
	{
		onItemTransformUpdate(item->id());
	}

	void Component::onItemTransformUpdate(QUuid itemKey)
	{
		if (contains(itemKey) && _pagePtr != NULL)
		{
			_pagePtr->onItemTransformUpdate(itemKey);
		}
	}
}
