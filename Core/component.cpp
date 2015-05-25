#include "component.h"

namespace CDI
{
	Component::Component()
		: AbstractModelItem()
	{

	}

	QTransform Component::itemTransform(AbstractModelItem* item) const
	{
		return transform() * (item->transform());
	}

	QTransform Component::itemTransform(QString itemId) const
	{
		QList<AbstractModelItem*> list_of_items = values(itemId);
		if (list_of_items.size())
			return transform() * (list_of_items[0]->transform());
		return transform();
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
			QHash<QString, AbstractModelItem*>::const_iterator iter;
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
			ptr = new Component();
		default :
			qDebug() << "Default constructor for given itemtype is not available. Returning NULL";
		}
		return ptr;
	}
}
