#pragma once
#include "commonfunctions.h"
#include "abstractmodelitem.h"
#include "component.h"

namespace CDI
{
	class Assembly : public AbstractModelItem
	{
	protected:
		QVector<Component*> components;
	public:
		Assembly();
		~Assembly();

		ItemType type() const;

		QDataStream& serialize(QDataStream& stream) const ;
		QDataStream& deserialize(QDataStream& stream);
	};
}
