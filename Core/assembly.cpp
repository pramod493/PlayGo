#include "assembly.h"

namespace CDI
{
	Assembly::Assembly(QGraphicsItem* parent)
		: QGraphicsItemGroup(parent)
	{
		_id = uniqueHash();
		_data = new AssemblyData;
	}

	Assembly::~Assembly()
	{
		delete _data;
	}

	void Assembly::addComponent(Component* component)
	{
		if (_data->components.contains(component->id()) == false)
		{
			_data->components.insert(component->id(), component);
		}
		addToGroup(component);
	}

	void Assembly::removeComponent(Component* component)
	{
		if (_data->components.contains(component->id()))
		{
			_data->components.remove(component->id());
		}
		removeFromGroup(component);
	}

	void Assembly::removeComponent(QUuid componentId)
	{
		if (_data->components.contains(componentId))
		{
			Component* component = _data->components.value(componentId);
			_data->components.remove(componentId);
			removeFromGroup(component);
		}
	}

	bool Assembly::containsComponent(QUuid componentId)
	{
		return _data->components.contains(componentId);
	}

	QList<Component*> Assembly::components()
	{
		return _data->components.values();
	}

	bool Assembly::mergeAssembly(Assembly *a)
	{
		Q_UNUSED(a)
		return false;
	}

	Component* Assembly::getComponentById(QUuid componentId)
	{
		if (_data->components.contains(componentId))
			return _data->components.value(componentId);
		return NULL;
	}

	void Assembly::addJoint(Component *c1, Component *c2)
	{
		Q_UNUSED(c1)
		Q_UNUSED(c2)
	}

	QDataStream& Assembly::serialize(QDataStream& stream) const
	{
		stream << _id;
		return stream;
	}

	QDataStream& Assembly::deserialize(QDataStream& stream)
	{
		stream >> _id;
		return stream;
	}

}
