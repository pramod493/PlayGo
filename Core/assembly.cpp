#include "assembly.h"

namespace CDI
{
	Assembly::Assembly()
	{
		_rootPtr = NULL;
		_assemblyFilepath = id().toString();
	}

	Assembly::~Assembly()
	{
		QHash<QUuid, Component*>::const_iterator iter;
		for(iter = constBegin(); iter != constEnd(); ++iter)
		{
			Component* component = iter.value();
			delete component;
		}
	}

	QTransform Assembly::transform() const
	{
		return _transform;
	}

	/*************************************************************
	 * Query functions
	 ************************************************************/
	ItemType Assembly::type() const
	{
		return ItemType::ASSEMBLY;
	}

	void Assembly::addItem(Component *component)
	{
		if (contains(component->id())) return;
		insert(component->id(), component);
	}

	void Assembly::removeItem(Component *component)
	{
		removeItem(component->id());
	}

	void Assembly::removeItem(QUuid uid)
	{
		if (contains(uid))
		{
			remove(uid);
		}
	}

	void Assembly::deleteitem(Component *component)
	{
		removeItem(component);
		delete component;
	}

	bool Assembly::containsItem(AbstractModelItem* key, bool searchRecursive)
	{
		return containsItem(key->id(), searchRecursive);
	}

	bool Assembly::containsItem(QUuid key, bool searchRecursive)
	{
		if (contains(key)) return true;
		if (searchRecursive)
		{
			QHash<QUuid, Component*>::const_iterator iter;
			for (iter = constBegin(); iter != constEnd(); ++iter)
			{
				Component* component = iter.value();
				if (component->containsItem(key))
					return true;
			}
		}
		return false;
	}

	bool Assembly::containsItem(QString key, bool searchRecursive)
	{
		QUuid uid = QUuid(key);
		if (uid.isNull()) return false;
		return containsItem(uid, searchRecursive);
	}

	bool Assembly::readFromFile(QFile &file)
	{
		Q_UNUSED(file);
		// TODO - feature not implemented
		return false;
	}

	bool Assembly::writeToFile(QFile& file)
	{
		Q_UNUSED(file);
		// TODO - feature not implemented
		return false;
	}

	QDataStream& Assembly::serialize(QDataStream& stream) const
	{
		stream << size();
		if (!isEmpty())
		{
			QHash<QUuid, Component*>::const_iterator iter;
			for(iter = constBegin(); iter != constEnd(); ++iter)
			{
				Component* component = iter.value();
				stream << *component;
			}
		}
		return stream;
	}

	QDataStream& Assembly::deserialize(QDataStream& stream)
	{
		int num_items;
		stream >> num_items;
		if (num_items != 0)
		{
			reserve(num_items);
			for (int i =0; i< num_items; i++)
			{
				Component* component = new Component();
				stream >> *component;
				insert(component->id(), component);
			}
		}
		return stream;
	}
}
