#include "abstractmodelitem.h"
#include <QString>

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
}
