#include "assembly.h"
#include "component.h"
#include <QUuid>
namespace CDI
{
	class AssemblyPrivateData 
	{
		// Contains data private to assembly. 
		// cannot be accessed from outside

	public:
		QString datetime;
		QUuid id;
		QString assemblyname; /** < keep name [0-9,a-z] >*/

		QHash<QUuid, Component*> components;
		QHash<QUuid, QGraphicsItem*> joints;

		AssemblyPrivateData()
		{
			datetime = QString("current time");
			id = uniqueHash();
			assemblyname = "assembly";
		}

		QDataStream& serialize(QDataStream& stream) const
		{
			stream << datetime;
			stream << id;
			stream << assemblyname;
			return stream;
		}

		QDataStream& deserialize(QDataStream& stream)
		{
			stream >> datetime;
			stream >> id;
			stream >> assemblyname;
			return stream;
		}
	};

	Assembly::Assembly(QGraphicsItem* parent)
		: QGraphicsItemGroup(parent)
	{
		_id = uniqueHash();
		_data = std::unique_ptr<AssemblyPrivateData>();
	}
    /**
     * @brief Assembly::~Assembly
     */
	Assembly::~Assembly()
	{
		//delete _data; auto deletes
	}

	int Assembly::type() const
	{
		return Type; 
	}

	QUuid Assembly::id() const 
	{
		return _data->id;
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
		return nullptr;
	}

	void Assembly::addJoint(Component *c1, Component *c2)
	{
		Q_UNUSED(c1)
		Q_UNUSED(c2)
	}

	QDataStream& Assembly::serialize(QDataStream& stream) const
	{
		return _data->serialize(stream);
	}

	QDataStream& Assembly::deserialize(QDataStream& stream)
	{
		return _data->deserialize(stream);
	}

}
