#include "assembly.h"

namespace CDI
{
	Assembly::Assembly()
	{

	}

	Assembly::~Assembly()
	{

	}

	ItemType Assembly::type() const
	{
		return ItemType::ASSEMBLY;
	}

	QDataStream& Assembly::serialize(QDataStream& stream) const
	{
		return stream;
	}

	QDataStream& Assembly::deserialize(QDataStream& stream)
	{
		return stream;
	}
}
