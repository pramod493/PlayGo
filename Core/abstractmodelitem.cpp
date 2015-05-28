#include <QString>
#include "abstractmodelitem.h"
#include "stroke.h"
#include "image.h"
#include "polygon2d.h"
#include "component.h"
#include "assembly.h"

namespace CDI
{
	QDataStream& AbstractModelItem::serializeInternal(QDataStream& stream) const
	{
		stream << itemId;
		return this->serialize(stream);
	}

	QDataStream& AbstractModelItem::deserializeInternal(QDataStream& stream)
	{
		stream >> itemId;
		return this->deserialize(stream);
	}

	QDataStream& operator<<(QDataStream& stream, const AbstractModelItem& item)
	{
		return item.serializeInternal(stream);
	}

	QDataStream& operator>>(QDataStream& stream, AbstractModelItem& item)
	{
		return item.deserializeInternal(stream);
	}

	AbstractModelItem* getItemPtrByType(ItemType t)
	{
		// NOTE - In case int is available, use getItemType(int i)
		// to get the ItemType

		AbstractModelItem* ptr = NULL;
		switch (t)
		{
		case ItemType::STROKE :
			ptr = new Stroke();
			break;
		case ItemType::IMAGE :
			ptr = new Image();
			break;
		case ItemType::POLYGON2D :
			ptr = new Polygon2D();
		case ItemType::COMPONENT :
			ptr = new Component();
		case ItemType::ASSEMBLY :
			ptr = new Assembly();
		default :
			qDebug() << "Default constructor for given itemtype is not available. Returning NULL";
		}
		return ptr;
	}
}
