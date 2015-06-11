#include "component.h"
#include "page.h"

namespace CDI
{
	Component::Component(Page *parent)
		: AbstractModelItem()
	{
		_pagePtr = parent;
	}

	QTransform Component::transform() const
	{
		return _transform;
	}

	QTransform Component::itemTransform(AbstractModelItem* item) const
	{
		return transform() * (item->transform());
	}

	QTransform Component::itemTransform(QUuid itemId) const
	{
		QTransform t = transform();
		QList<AbstractModelItem*> list_of_items = values(itemId);
		if (list_of_items.size())
			t = t * (list_of_items[0]->transform());
		return t;
	}

	void Component::setTransform(QTransform& transform, bool combine)
	{
		mask |= isTransformed;
		_transform = (combine ? _transform * transform : transform);
	}

	Stroke* Component::addStroke(QColor color, float thickness)
	{
		Stroke* stroke = new Stroke(color, thickness);
		addItem(stroke);
		return stroke;
	}

	Image* Component::addImage(const QString filename)
	{
		Image* image = new Image(filename);
		addItem(image);
		return image;
	}

	void Component::addItem(AbstractModelItem* item)
	{
		insert(item->id().toString(), item);
	}

	bool Component::removeItem(AbstractModelItem* item)
	{
		return ((remove(item->id())!=0) ? true : false);
	}

	bool Component::containsItem(AbstractModelItem *key, bool searchRecursive)
	{
		return containsItem(key->id(), searchRecursive);
	}

	bool Component::containsItem(QUuid key, bool searchRecurive)
	{
		if (contains(key)) return true;
		if (searchRecurive)
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
		}
		return false;
	}

	bool Component::containsItem(QString key, bool searchRecursive)
	{
		QUuid id = QUuid(key);
		if (id.isNull()) return false;	// INVALID ID
		return containsItem(id, searchRecursive);
	}

	AbstractModelItem* Component::getItemPtrById(QUuid id, bool searchRecursive)
	{
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
		return NULL;
	}

	ItemType Component::type() const
	{
		return ItemType::COMPONENT;
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
			ptr = new Stroke();
			break;
		case ItemType::IMAGE :
			qDebug() << "Creating empty Image object";
			ptr = new Image();
			break;
		case ItemType::COMPONENT :
			qDebug() << "Components are not supposed to contain components"
					 << "@Component::createEmptyItem(ItemType): AbstractModelItem*";
			ptr = _pagePtr->createComponent();
		default :
			qDebug() << "Default constructor for given itemtype is not available. Returning NULL";
		}
		return ptr;
	}
}
