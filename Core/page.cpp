#include "page.h"
#include "playgo.h"
#include <QtAlgorithms>

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
	}

	Page::~Page()
	{
		// Delete all children
		// 1. Remove all components which are in assembly from the list
		// NOTE : Let us not worry about deletion of QHash. Also, forget about
		// updating the transform because we already going to delete them anyways
		// Simple deletion should take care of it
		//		for (QHash<QUuid, Assembly*>::const_iterator iter = _assemblies.constBegin();
		//			 iter != _assemblies.constEnd(); ++iter)
		//		{
		//			Assembly* assembly = iter.value();
		//			QHash<QUuid, Component*>::const_iterator comp_iter;
		//			for (comp_iter = assembly->constBegin();
		//				 comp_iter != assembly->constEnd(); ++comp_iter)
		//			{
		//				Component* component = comp_iter.value();
		//				if (_components.contains(component->id()))
		//					_components.remove(component->id());
		//			}
		//		}

		for (QHash<QUuid, Assembly*>::const_iterator
			 iter = _assemblies.constBegin();
			 iter != _assemblies.constEnd();
			 ++iter)
		{
			Assembly* assembly = iter.value();

			delete assembly;
		}

		for (QHash<QUuid, Component*>::const_iterator
			 comp_iter = _components.constBegin();
			 comp_iter != _components.constEnd();
			 ++comp_iter)
		{
			Component* component = comp_iter.value();
			delete component;
		}
	}

	void Page::deleteAll()
	{
		QHash<QUuid, Assembly*>::const_iterator iter;
		for (iter = _assemblies.constBegin();
			 iter != _assemblies.constEnd(); ++iter)
		{
			destroyAssembly(iter.value());
		}

		QHash<QUuid, Component*>::const_iterator comp_iter;
		for (comp_iter = _components.constBegin();
			 comp_iter != _components.constEnd(); ++iter)
		{
			destroyComponent(comp_iter.value());
		}
	}

	bool Page::add(Page* page)
	{
		qDebug() << "Feature not implemented";
		if (page== NULL) qDebug() <<"Not cool. NULL Page pointer@Page::add()";
		return false;
	}

	Assembly* Page::createAssembly()
	{
		Assembly* newAssembly = new Assembly(this);
		_assemblies.insert(newAssembly->id(), newAssembly);
		return newAssembly;
	}

	Component* Page::createComponent()
	{
		Component* newComponent = new Component(this);
		_components.insert(newComponent->id(), newComponent);
		return newComponent;
	}

	QList<Assembly*> Page::getAssemblies() const
	{
		return _assemblies.values();
	}

	QList<Component*> Page::getComponents() const
	{
		return _components.values();
	}

	bool Page::mergeAssembly(Assembly *a1, Assembly *a2)
	{
		if (a1 != NULL && a2 != NULL )
			if (!(a2->isEmpty() || a1->isEmpty()))
				if (a1->mergeAssembly(a2))
				{
					emit onAssemblyMerge(a1,a2);
					if (_assemblies.contains(a2->id()))
						_assemblies.remove(a2->id());

					delete a2;
					return true;
				}
		return false;
	}

	// This deletes the components as well
	bool Page::destroyAssembly(Assembly *assembly)
	{
		if (assembly == NULL) return false;
		if (_assemblies.contains(assembly->id()))
		{
			emit beforeAssemblyDeletion(assembly);	// This should take care of deletion of components as well
			// remove all component references from hash
			QHash<QUuid, Component*>::const_iterator iter;
			for(iter = assembly->constBegin(); iter != assembly->constEnd(); ++iter)
			{
				Component* component = iter.value();
				if (component!= NULL)
				{
					emit beforeComponentDeletion(component);
					_components.remove(component->id());
					delete component;
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
			qDebug() << "Component is NULL";
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
			emit beforeComponentDeletion(component);
			delete component;
		}
		return markForDelete;
	}

	void Page::triggerUpdate(AbstractModelItem* item)
	{
		if (item->type() == ItemType::ASSEMBLY)
		{
			emit onAssemblyUpdate(static_cast<Assembly*>(item));
			return;
		}
		if (item->type() == ItemType::COMPONENT)
		{
			emit onComponentUpdate(static_cast<Component*>(item));
			return;
		}
		AbstractModelItem* parent = getParent(item);
		if (parent != NULL) triggerUpdate(parent);
	}

	AbstractModelItem* Page::getParent(QUuid id)
	{
		AbstractModelItem* parentItem = NULL;
		// 1. Check if item is an assembly
		if (_assemblies.contains(id)) return parentItem;

		// 2. Search in immediate children of assemblies
		QHash<QUuid, Assembly*>::const_iterator assemiter;
		for (assemiter = _assemblies.constBegin();
			 assemiter != _assemblies.constEnd(); ++assemiter)
		{
			Assembly* assembly = assemiter.value();
			if (assembly->contains(id))
			{
				return assembly;
				break;
			}
		}

		// 3. Search in children of components
		QHash<QUuid, Component*>::const_iterator componentiter;
		for (componentiter = _components.constBegin();
			 componentiter != _components.constEnd(); ++componentiter)
		{
			Component* component = componentiter.value();
			if (component->containsItem(id, false))
				return component;
		}

		// 4. If item is not found this way, perform a deep search into all components
		for (componentiter = _components.constBegin();
			 componentiter != _components.constEnd(); ++componentiter)
		{
			Component* component = componentiter.value();
			if (component->containsItem(id, true))		// Perform a recursive search
				return component;
		}

		return parentItem;
	}

	AbstractModelItem* Page::getParent(AbstractModelItem *item)
	{
		if (item->type() == ItemType::ASSEMBLY) return NULL;
		if (item->type() == ItemType::COMPONENT)
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
		}

		return NULL;
	}

	bool Page::contains(QUuid id, bool searchRecursive)
	{
		// Check immediate objects
		if (_assemblies.contains(id)) return true;
		if (_components.contains(id)) return true;

		if (searchRecursive)
		{
			QHash<QUuid, Assembly*>::const_iterator assemiter;
			for (assemiter = _assemblies.constBegin();
				 assemiter != _assemblies.constEnd(); ++assemiter)
			{
				Assembly* assembly = assemiter.value();
				if (assembly->contains(id)) return true;
			}
		}
		QHash<QUuid, Component*>::const_iterator componentiter;
		for (componentiter = _components.constBegin();
			 componentiter != _components.constEnd(); ++componentiter)
		{
			Component* component = componentiter.value();
			if (component->containsItem(id, searchRecursive))
				return true;
		}
		return false;
	}

	QDataStream& Page::serialize(QDataStream& stream) const
	{
		// Make sure not to serialize components which have already been serialized by the assembly;
		return stream;
	}

	QDataStream& Page::deserialize(QDataStream& stream)
	{
		return stream;
	}

	QDataStream& operator<<(QDataStream& stream, const Page& page)
	{
		return page.serialize(stream);
	}

	QDataStream& operator>>(QDataStream& stream, Page& page)
	{
		return page.deserialize(stream);
	}
}
