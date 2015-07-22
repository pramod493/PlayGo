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
		settings.gravity = Point2D(0,0);
		settings.timeStep = 1.0f/60.0f;
		_physicsManager = new PhysicsManager(&settings, this);

		_scene = NULL;

		_currentComponent = NULL;

		transformedComponents = QList<Component*>();

		connect(this, SIGNAL(signalItemAdd(QGraphicsItem*)),
				_physicsManager, SLOT(onItemAdd(QGraphicsItem*)));
		connect(_physicsManager, SIGNAL(physicsStepComplete()),
				this, SLOT(onSimulationStepComplete()));
		connect(_physicsManager, SIGNAL(physicsStepStart()),
				this, SLOT(onSimulationStepStart()));

	}

	Page::~Page()
	{
		deleteAll();

		if (_searchManager) delete _searchManager;
		if (_physicsManager) delete _physicsManager;
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

	QGraphicsScene* Page::scene() const
	{
		return _scene;
	}

	void Page::setScene(QGraphicsScene *scene)
	{
		_scene = scene;
		if (_scene == NULL) return;

		QHash<QUuid, Component*>::const_iterator componentiter;
		for (componentiter = _components.constBegin();
			 componentiter != _components.constEnd(); ++componentiter)
		{
			Component* component = componentiter.value();
			_scene->addItem(component);
		}
		QHash<QUuid, Assembly*>::const_iterator assemiter;
		for (assemiter = _assemblies.constBegin();
			 assemiter != _assemblies.constEnd(); ++assemiter)
		{
			Assembly* assembly = assemiter.value();
			_scene->addItem(assembly);
		}
	}

	Component* Page::currentComponent()
	{
		return _currentComponent;
	}

	void Page::setCurrentComponent(Component *currentComp)
	{
		if (currentComp == NULL)
		{
			_currentComponent = NULL; return;
		}

		if (currentComp == _currentComponent) return;

		if (_components.contains(currentComp->id()) == false)
		{
			addComponent(currentComp);
		}

		_currentComponent = currentComp;
	}

	QGraphicsItem* Page::getItemPtrById(QUuid id)
	{
		// NOTE - No check has been to verify if the item is of type
		// Component or Assembly
		// Only checking among components right now
		QHash<QUuid, Component*>::const_iterator componentiter;
		for (componentiter = _components.constBegin();
			 componentiter != _components.constEnd(); ++componentiter)
		{
			Component* component = componentiter.value();
			if (component->id() == id) return component;

			if (component->containsItem(id))
				return component->getItemById(id);
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

	bool Page::contains(QUuid id)
	{
		// Check immediate objects
		if (_assemblies.contains(id)) return true;
		if (_components.contains(id)) return true;

		QHash<QUuid, Component*>::const_iterator componentiter;
		for (componentiter = _components.constBegin();
			 componentiter != _components.constEnd(); ++componentiter)
		{
			Component* component = componentiter.value();
			if (component->containsItem(id))
				return true;
		}

		// TODO - Check within assembly first
		return false;
	}

	ItemType Page::type() const
	{
		return PAGE;
	}

	Assembly* Page::createAssembly()
	{
		Assembly* newAssembly = new Assembly();
		addAssembly(newAssembly);
		return newAssembly;
	}

	Component* Page::createComponent()
	{
		Component* newComponent = new Component();
		addComponent(newComponent);
		return newComponent;
	}

	void Page::addComponent(Component *component)
	{
		if (_components.contains(component->id())) return;
		_components.insert(component->id(), component);
		if (_scene) _scene->addItem(component);
		initializeComponentConnections(component);
		emit signalItemAdd(component);
	}

	void Page::addAssembly(Assembly *assembly)
	{
		if (_assemblies.contains(assembly->id())) return;
		_assemblies.insert(assembly->id(), assembly);
		if (_scene) _scene->addItem(assembly);
		emit signalItemAdd(assembly);
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
			QList<Component*> lis_components = assembly->components();

			for (QList<Component*>::const_iterator iter = lis_components.constBegin();
				 iter != lis_components.constEnd(); ++iter)
			{
				Component* component = (*iter);
				_components.remove(component->id());
				assembly->removeComponent(component);
				delete component;
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
			QLOG_ERROR() << "Invalid component pointer";
			return false;
		}

		// 0. Remove the component from component hash
		if (_components.contains(component->id()))
		{
			markForDelete = true;
			_components.remove(component->id());
			removeComponentConnections(component);
		}

		// Remove it from temp list
		if (transformedComponents.contains(component))
		{
			transformedComponents.removeOne(component);
		}

		if (_currentComponent == component)
		{
			_currentComponent = NULL;
		}

		// 1. Find the assembly which contains the components
		QHash<QUuid, Assembly*>::const_iterator assemiter;
		for (assemiter = _assemblies.constBegin();
			 assemiter != _assemblies.constEnd(); ++assemiter)
		{
			Assembly* assembly = assemiter.value();
			if (assembly->containsComponent(component->id()))
			{
				markForDelete = true;
				assembly->removeComponent(component);
				break;
			}
		}
		if (markForDelete)
		{
			emit signalItemDelete(component);
			QList<QGraphicsItem*> childItems = component->childItems();
			b2Body* physicsBody = component->physicsBody();
			if (physicsBody)
			{
				foreach (PhysicsJoint* joint, component->_jointlist)
				{
					_physicsManager->deleteJoint(joint);
				}
				component->_jointlist.clear();

				foreach (b2Fixture* fixture, component->_fixtures)
				{
					physicsBody->DestroyFixture(fixture);
				}
				component->_fixtures.clear();

				_physicsManager->destroyBody(component);
				component->_physicsBody = NULL;
			}

			qDeleteAll(childItems);

			delete component;
		}
		return markForDelete;
	}

	bool Page::add(Page* page)
	{
		QLOG_TRACE() << "Feature not implemented";
		if (page== NULL) QLOG_INFO() <<"Not cool. NULL Page pointer@Page::add()";
		return false;
	}

	QDataStream& Page::serialize(QDataStream& stream) const
	{
		stream << _id;
		// Make sure not to serialize components which have already been serialized by the assembly;
		// Implement saving of component data only
		int num_components = _components.size();
		int num_assemblies = _assemblies.size();
		stream << num_components;
		stream << num_assemblies;
		if (num_components == 0 && num_assemblies == 0) return stream;

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

		_id = newId;

		int num_components = 0;
		int num_assemblies = 0;
		stream >> num_components;
		stream >> num_assemblies;		// Do not write assembly info now.

		if (num_components != 0)
		{
			for (int i=0; i<num_components; i++)
			{
				Component *component = new Component();
				component->deserialize(stream);
				addComponent(component);
			}
		}
		emit signalReloadPage(this);

		return stream;
	}

	void Page::deleteAll()
	{
		// Do not iterate over hash to delete items. get the list of objects and delete them
		QList<Assembly*> assemblies = _assemblies.values();
		for (int i=0; i < assemblies.size(); i++)
		{
			Assembly* assembly = assemblies[i];
			destroyAssembly(assembly);
		}
		_assemblies.clear();

		QList<Component*> components = _components.values();
		for (int i=0; i < components.size(); i++)
		{
			Component* component  = components[i];
			destroyComponent(component);
		}
		_components.clear();
	}

	void Page::initializeComponentConnections(Component *component)
	{
		// add other connections as well
		connect(component, SIGNAL(onTransformChange(QGraphicsItem*)),
				this, SLOT(onItemTransformUpdate(QGraphicsItem*)));
	}

	void Page::removeComponentConnections(Component *component)
	{
		// Right now nothing to do here
	}

	// Slots
	void Page::onItemAdd(QGraphicsItem* graphicsitem)
	{
		emit signalItemAdd(graphicsitem);
	}

	void Page::onItemUpdate(QGraphicsItem* graphicsitem)
	{
		emit signalItemUpdate(graphicsitem);
	}

	void Page::onItemDelete(QGraphicsItem* graphicsitem)
	{
		emit signalItemDelete(graphicsitem);
	}

	void Page::onItemTransformUpdate(QGraphicsItem* graphicsitem)
	{
		if (graphicsitem->type() == Component::Type)
			onComponentTransformUpdate
					(qgraphicsitem_cast<Component*>(graphicsitem));
		emit signalItemTransformUpdate(graphicsitem);
	}

	void Page::onItemAdd(Item* item)
	{
		emit signalItemAdd(item);
	}

	void Page::onItemUpdate(Item* item)
	{
		emit signalItemUpdate(item);
	}

	void Page::onItemDelete(Item* item)
	{
		emit signalItemDelete(item);
	}

	void Page::onItemTransformUpdate(Item* item)
	{
		emit signalItemTransformUpdate(item);
	}

	void Page::onSimulationStepStart()
	{
		if (_physicsManager)
			_physicsManager->updateFromComponentPosition(transformedComponents);
	}

	void Page::onSimulationStepComplete()
	{
		QList<Component*> lis_comps = _components.values();
		if (_physicsManager)
			_physicsManager->updateComponentPosition(lis_comps);
		if (_scene)	_scene->update();
	}

	void Page::onComponentTransformUpdate(Component *component)
	{
		// Batches the transformation behavior which updates the
		// box2d prior to step()
		if (transformedComponents.contains(component) == false)
			transformedComponents.push_back(component);
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
