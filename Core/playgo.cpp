#include "playgo.h"

namespace CDI
{
	PlayGo::PlayGo()
	{
		assemblies = QHash<QUuid, Assembly*>();
		freeComponents = QHash<QUuid, Component*>();
	}

	PlayGo::~PlayGo()
	{
		deleteAllItems();
	}

	void PlayGo::deleteAllItems()
	{
		// delete all components as well as free items
		if (!assemblies.isEmpty())
		{
			QHash<QUuid, Assembly*>::const_iterator iter;
			for (iter = assemblies.constBegin();
				 iter != assemblies.constEnd(); ++iter)
			{
				Assembly* assembly = iter.value();
				if (assembly != NULL) delete assembly;
			}
		}

		if (!freeComponents.isEmpty())
		{
			QHash<QUuid, Component*>::const_iterator iter;
			for (iter = freeComponents.constBegin();
				 iter != freeComponents.constEnd(); ++iter)
			{
				Component* item = iter.value();
				if (item != NULL) delete item;
			}
		}
	}

	Component* PlayGo::addComponent(Assembly *parent)
	{
		Component* component = new Component();
		parent->addItem(component);
		return component;
	}

	Assembly* PlayGo::addAssembly()
	{
		Assembly* assembly = new Assembly();
		assemblies.insert(assembly->id(), assembly);
		return assembly;
	}

	bool PlayGo::contains(Assembly *assembly)
	{
		// Lookup using QUuid
		return contains(assembly->id(), false);
	}

	bool PlayGo::contains(QUuid id, bool searchRecursive)
	{
		if (assemblies.contains(id)) return true;
		if (searchRecursive)
		{
			QHash<QUuid, Component*>::const_iterator iter;
			for (iter = freeComponents.constBegin();
				 iter != freeComponents.constEnd(); ++iter)
			{

			}
		}
		return false;
	}

	bool PlayGo::contains(QString id, bool searchRecursive)
	{
		QUuid uid = QUuid(id);
		if (uid.isNull()) return false;
		return contains(uid, searchRecursive);
	}

	Assembly* PlayGo::getAssemblyById(QUuid id)
	{
		// TODO - Feeling too lazy to implement now
		return NULL;
	}

	Assembly* PlayGo::getAssemblyById(QString id)
	{
		QUuid uid = QUuid(id);
		if (uid.isNull()) return false;
		return getAssemblyById(uid);
	}

	QDataStream& PlayGo::serialize(QDataStream& stream) const
	{
		{
			stream << assemblies.size();
			QHash<QUuid, Assembly*>::const_iterator iter;
			for (iter = assemblies.constBegin();
				 iter != assemblies.constEnd(); ++iter)
			{
				Assembly *assembly = iter.value();
				stream << *assembly;
			}
		}
		{
			stream << freeComponents.size();
			QHash<QUuid, Component*>::const_iterator iter;
			for (iter = freeComponents.constBegin();
				 iter != freeComponents.constEnd(); ++iter)
			{
				Component *item = iter.value();
				stream << *item;
			}
		}
		return stream;
	}

	QDataStream& PlayGo::deserialize(QDataStream& stream)
	{
		int num_assemblies;
		stream >> num_assemblies;
		for (int i=0; i< num_assemblies; i++)
		{
			Assembly *assembly = new Assembly();
			stream >> *assembly;
			assemblies.insert(assembly->id(), assembly);
		}

		int num_components;
		stream >> num_components;
		for (int i=0; i< num_components; i++)
		{
			Component *component = new Component;
			stream >> *component;
			freeComponents.insert(component->id(), component);
		}
		return stream;
	}

	QDataStream& operator>>(QDataStream& stream, const PlayGo& item)
	{
		return item.serialize(stream);
	}

	QDataStream& operator<<(QDataStream& stream, PlayGo& item)
	{
		return item.deserialize(stream);
	}
}

