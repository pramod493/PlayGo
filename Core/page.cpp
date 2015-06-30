#include "page.h"
#include "playgo.h"
#include <QtAlgorithms>
#include "QsLog.h"

namespace CDI
{
	// This cannot exist on its own. It must have a parent PlayGo. Though not sure
	// what will happen when its missing. we don't seem to use it often
	Page::Page(PlayGo* parent)
		: QObject(parent)
	{
		_id = uniqueHash();
		_playgo = parent;

		_assemblies = QHash<QUuid, Assembly*>();
		_components = QHash<QUuid, Component*>();

		_searchManager = new SearchManager(this);

		PhysicsSettings settings = PhysicsSettings();
		_physicsManager = new PhysicsManager(&settings, this);
	}

	Page::~Page()
	{
		deleteAll();
	}

	QUuid Page::id() const
	{
		return _id;
	}

	SearchManager* Page::getSearchManager() const
	{
		return _searchManager;
	}

	PhysicsManager* Page::getPhysicsManager() const
	{
		return _physicsManager;
	}

	QList<Assembly*> Page::getAssemblies() const
	{
		return _assemblies.values();
	}

	QList<Component*> Page::getComponents() const
	{
		return _components.values();
	}

	AbstractModelItem* Page::getItemPtrById(QUuid id)
	{
		// NOTE - No check has been to verify if the item is of type
		// Component or Assembly
		// Only checking among components right now
		QHash<QUuid, Component*>::const_iterator componentiter;
		for (componentiter = _components.constBegin();
			 componentiter != _components.constEnd(); ++componentiter)
		{
			Component* component = componentiter.value();
			if (component->containsItem(id))
				return component->getItemPtrById(id);
		}
		return NULL;
	}

	Component* Page::getComponentPtrById(QUuid id)
	{
		if (_components.contains(id))
			return _components.value(id);
		return NULL;
	}

	Assembly* Page::getAssemblyPtrById(QUuid id)
	{
		if (_assemblies.contains(id))
			return _assemblies.value(id);
		return NULL;
	}

	ItemType Page::getItemType(QUuid id)
	{
		if (_assemblies.contains(id)) return ASSEMBLY;
		if (_components.contains(id)) return COMPONENT;
		AbstractModelItem* itemPtr = getItemPtrById(id);
		if (itemPtr == NULL) return NONE;
		return itemPtr->type();
	}

	/*AbstractModelItem* Page::getParent(AbstractModelItem *item)
	{

		if (item->type() == ASSEMBLY) return NULL;
		if (item->type() == COMPONENT)
		{
			QHash<QUuid, Assembly*>::const_iterator assemiter;
			for (assemiter = _assemblies.constBegin();
				 assemiter != _assemblies.constEnd(); ++assemiter)
			{
				Assembly* assembly = assemiter.value();
				if (assembly->contains(item->id()))
				{
					return assembly;
				}
			}
		} else {
			// Look into components
			QHash<QUuid, Component*>::const_iterator componentiter;
			for (componentiter = _components.constBegin();
				 componentiter != _components.constEnd(); ++componentiter)
			{
				Component* component = componentiter.value();
				if (component->containsItem(item, false))
					return component;
			}
		}
		return NULL;
	}*/

	bool Page::contains(QUuid id)
	{
		// Check immediate objects
		if (_assemblies.contains(id)) return true;
		if (_components.contains(id)) return true;

		/*if (searchRecursive)
		{
			QHash<QUuid, Assembly*>::const_iterator assemiter;
			for (assemiter = _assemblies.constBegin();
				 assemiter != _assemblies.constEnd(); ++assemiter)
			{
				Assembly* assembly = assemiter.value();
				if (assembly->contains(id)) return true;
			}
		}*/
		QHash<QUuid, Component*>::const_iterator componentiter;
		for (componentiter = _components.constBegin();
			 componentiter != _components.constEnd(); ++componentiter)
		{
			Component* component = componentiter.value();
			if (component->containsItem(id))
				return true;
		}
		return false;
	}

	ItemType Page::type() const
	{
		return PAGE;
	}

	Assembly* Page::createAssembly()
	{
		Assembly* newAssembly = new Assembly(this);
		_assemblies.insert(newAssembly->id(), newAssembly);

		emit signalAssemblyAdd(newAssembly);

		return newAssembly;
	}

	Component* Page::createComponent()
	{
		Component* newComponent = new Component(this);
		_components.insert(newComponent->id(), newComponent);

		emit signalComponentAdd(newComponent);

		return newComponent;
	}

	void Page::addComponent(Component *component)
	{
		if (_components.contains(component->id()) == false)
		{
			_components.insert(component->id(), component);
			emit signalComponentAdd(component);
		}

	}

	void Page::addAssembly(Assembly *assembly)
	{
		if (_assemblies.contains(assembly->id()) == false)
		{
			_assemblies.insert(assembly->id(), assembly);
			emit signalAssemblyAdd(assembly);
		}
	}

	bool Page::mergeAssembly(Assembly *a1, Assembly *a2)
	{
		if (_assemblies.contains(a1->id()) && _assemblies.contains(a2->id()))
			return a1->mergeAssembly(a2);
		return false;
	}

	// This deletes the components as well
	bool Page::destroyAssembly(Assembly *assembly)
	{
		if (assembly == NULL) return false;
		if (_assemblies.contains(assembly->id()))
		{

			/*QHash<QUuid, Component*>*/ItemHash::const_iterator iter;
			for(iter = assembly->constBegin(); iter != assembly->constEnd(); ++iter)
			{
				Item* item = iter.value();
				// TODO - How to manage others?
				// Also can we keep components in the same league as other <Item>
				if (item->type() == COMPONENT)
				{
					Component* component = static_cast<Component*>(item);
					_components.remove(component->id());
					delete component;
				} else {
					delete item;
				}
			}
			_assemblies.remove(assembly->id());
			delete assembly;
			return true;
		}
		return false;
	}

	bool Page::destroyComponent(Component *component)
	{
		// Do not delete the component if it is not contained in the page
		bool markForDelete = false;

		if (component == NULL)
		{
			QLOG_INFO() << "Component is NULL";
			return markForDelete;
		}

		// 0. Remove the component from component hash
		if (_components.contains(component->id()))
		{
			markForDelete = true;
			_components.remove(component->id());
		}
		// 1. Find the assembly which contains the components
		QHash<QUuid, Assembly*>::const_iterator assemiter;
		for (assemiter = _assemblies.constBegin();
			 assemiter != _assemblies.constEnd(); ++assemiter)
		{
			Assembly* assembly = assemiter.value();
			if (assembly->contains(component->id()))
			{
				markForDelete = true;
				assembly->removeItem(component->id());
				break;
			}
		}
		if (markForDelete)
		{
			emit signalComponentDelete(component);
			delete component;
		}
		return markForDelete;
	}

	bool Page::add(Page* page)
	{
		QLOG_INFO() << "Feature not implemented";
		if (page== NULL) QLOG_INFO() <<"Not cool. NULL Page pointer@Page::add()";
		return false;
	}

	QDataStream& Page::serialize(QDataStream& stream) const
	{
		stream << _id;
		// Make sure not to serialize components which have already been serialized by the assembly;
		// Implement saving of component data only
		int num_components = _components.size();
		stream << num_components;
		if (num_components == 0) return stream;

		QHash<QUuid, Component*>::const_iterator componentiter;
		for (componentiter = _components.constBegin();
			 componentiter != _components.constEnd(); ++componentiter)
		{
			Component* component = componentiter.value();
			component->serialize(stream);
		}
		return stream;
	}

	QDataStream& Page::deserialize(QDataStream& stream)
	{
		QUuid newId;
		stream >> newId;
		emit signalItemIdUpdate(_id, newId);
		_id = newId;

		int num_components = 0;
		stream >> num_components;
		if (num_components != 0)
		{
			for (int i=0; i<num_components; i++)
			{
				Component *component = new Component(this);
				component->deserialize(stream);
				addComponent(component);
			}
		}
		emit signalReloadPage(this);

		return stream;
	}

	void Page::deleteAll()
	{
		emit signalDeleteAllItems(this);
		// Destroys the relationship between components
		qDeleteAll(_assemblies);
		// Destroys all the components
		qDeleteAll(_components);

		_components.clear();
		_assemblies.clear();
	}

	void Page::onItemAdd(AbstractModelItem *item)
	{
		emit signalItemAdd(item);
	}

	void Page::onItemRemove(QUuid itemId)
	{
		emit signalItemRemove(itemId);
	}

	void Page::onItemUpdate(QUuid itemId)
	{
		emit signalItemUpdate(itemId);
	}

	void Page::onItemDisplayUpdate(QUuid itemId)
	{
		emit signalItemDisplayUpdate(itemId);
	}

	void Page::onItemRedraw(QUuid itemId)
	{
		emit signalItemRedraw(itemId);
	}

	void Page::onItemTransformUpdate(QUuid itemId)
	{
		emit signalItemTransformUpdate(itemId);
	}

	void Page::onItemIdUpdate(QUuid oldID, QUuid newID)
	{
		emit signalItemIdUpdate(oldID, newID);
	}
}
