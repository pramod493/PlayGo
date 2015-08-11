#include "forceitem.h"

namespace CDI
{
	ForceItem::ForceItem()
	{
		_id = uniqueHash();
		_component = NULL;
	}

	QDataStream& ForceItem::serialize(QDataStream &stream) const
	{
		stream << _id;
		return stream;
	}

	QDataStream& ForceItem::deserialize(QDataStream &stream)
	{
		stream >> _id;
		return stream;
	}
}
