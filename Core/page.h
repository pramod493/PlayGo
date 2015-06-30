#pragma once
#include <QDataStream>
#include <QObject>
#include <QList>
#include <QUuid>
#include <QtAlgorithms>

#include "commonfunctions.h"
#include "abstractmodelitem.h"
#include "assembly.h"
#include "component.h"
#include "searchmanager.h"
#include "physicsmanager.h"

namespace CDI
{
	class PlayGo;
	/**
	 * @brief The Page class is the top level item which contains self sufficient information regarding a scene.
	 * It also serves the function of factory class to create objects
	 * This is the top-level container for all AbstractModelItem objects
	 */
	class Page : public QObject, public Item
	{
		Q_OBJECT

	protected:
		/**
		 * @brief Pointer to root item of the model
		 */
		PlayGo *_playgo;

		/**
		 * @brief Page object ID
		 */
		QUuid _id;

		/**
		 * @brief _assemblies contains all the Assembly objects
		 * contained within the page.
		 */
		QHash<QUuid, Assembly*> _assemblies;

		/**
		 * @brief _components contains all the Component objects
		 * contained within the page including all the components
		 * which are part of a given Assembly
		 */
		QHash<QUuid, Component*> _components;

		/**
		 * @brief Pointer to search manager for this page
		 */
		SearchManager* _searchManager;

		/**
		 * @brief Pointer to physics manager for this page
		 */
		PhysicsManager* _physicsManager;

	public:
		//-----------------------------------------------
		// Constructors/Destructors
		//-----------------------------------------------
		Page(PlayGo* parent);

		virtual ~Page();



		//-----------------------------------------------
		// Query/Set functions(same order in derived class)
		// Non-virtual
		//-----------------------------------------------
		QUuid id() const;

		SearchManager* getSearchManager() const;

		PhysicsManager* getPhysicsManager() const;

		QList<Assembly*> getAssemblies() const;

		QList<Component*> getComponents() const;

		/**
		 * @brief getParent returns the immediate parent Component or
		 * Assembly of a given AbstractModelItem object
		 * In case no such parent is present, NULL is returned.
		 * In case the item is not present in the
		 * @param item
		 * @return parent object of the item
		 */
//		AbstractModelItem* getParent(QUuid id);

		AbstractModelItem* getItemPtrById(QUuid id);

		Component* getComponentPtrById(QUuid id);

		Assembly* getAssemblyPtrById(QUuid id);

//		AbstractModelItem* getParent(AbstractModelItem* item);

		/**
		 * @brief Returns the item type based on QUuid
		 * @param id QUuid
		 * @return ItemType of object which owns the id. Returns NONE in case of no match
		 */
		ItemType getItemType(QUuid id);

		/**
		 * @brief Checks if the given item is contained in the Page
		 * @param id
		 * @param searchRecursive
		 * @return
		 */
		bool contains(QUuid id);

		//-----------------------------------------------
		// Virtual functions (same order in derived class)
		//-----------------------------------------------
		ItemType type() const;

		virtual Assembly* createAssembly();
		virtual Component* createComponent();

		virtual void addComponent(Component* component);
		virtual void addAssembly(Assembly* assembly);

		// Merge a2 into a1
		virtual bool mergeAssembly(Assembly* a1, Assembly* a2);

		/**
		 * @brief Deletes all the components stored in an assembly as well
		 * as deletes the assembly.
		 * @param assembly
		 * @return True if deletion is successfull. False when the assembly
		 * is not a part of the page
		 */
		virtual bool destroyAssembly(Assembly* assembly);

		/**
		 * @brief Deletes all the items stored in the component as well
		 * as deletes the assembly.
		 * @param component
		 * @return True if deletion is successfull. False when the component
		 * is not a part of the page
		 */
		virtual bool destroyComponent(Component* component);

		/**
		 * @brief add copies the given page information into this page
		 * @param page: Page to add
		 * @return True on success, false on fail
		 */
		virtual bool add(Page* page);

		/**
		 * @brief Serializes page and its component
		 * @param stream
		 * @return
		 */
		virtual QDataStream& serialize(QDataStream &stream) const;

		/**
		 * @brief Deserializes page and re-creates components
		 * @param stream
		 * @return
		 */
		virtual QDataStream& deserialize(QDataStream &stream);

		//-----------------------------------------------
		// Other functions not related to query/set
		//-----------------------------------------------
		/**
		 * @brief Deletes all components and assemblies
		 */
		void deleteAll();

//		friend QDataStream& operator<<(QDataStream& stream, const Page& page);
//		friend QDataStream& operator>>(QDataStream& stream, Page& page);

		// These function are called on item updates
		// Note that if a component or assembly does not exist in the hash list
		// it's update signal will be discarded
	public slots:
		/**
		 * @brief Called when a new Item is added to the page.
		 * @param item
		 */
		void onItemAdd(AbstractModelItem* item);

		/**
		 * @brief Called when an item is either deleted or being
		 * removed fromthe page
		 * @param itemId QUuid of the item
		 */
		void onItemRemove(QUuid itemId);

		/**
		 * @brief Call when the component internals are updated
		 * @param itemId
		 */
		void onItemUpdate(QUuid itemId);

		void onItemDisplayUpdate(QUuid itemId);

		/**
		 * @brief Triggers redrawing of the object
		 * @param itemId
		 */
		void onItemRedraw(QUuid itemId);

		/**
		 * @brief Emits signalItemTransformUpdate() with the given item ID
		 * @param itemId Item ID
		 */
		void onItemTransformUpdate(QUuid itemId);

		void onItemIdUpdate(QUuid oldID, QUuid newID);

	signals:
		// Page related signals
		void signalDeleteAllItems(Page* page);

		void signalReloadPage(Page* page);

		void signalItemAdd(AbstractModelItem* item);

		void signalItemRemove(QUuid itemId);

		void signalItemUpdate(QUuid itemId);

		void signalItemDisplayUpdate(QUuid itemId);

		void signalItemRedraw(QUuid itemId);

		// For all type of Items
		void signalItemTransformUpdate(QUuid itemId);

		void signalItemIdUpdate(QUuid oldID, QUuid newID);

		// Component and Assembly related signals
		// Called only from within class
		void signalComponentAdd(Component* component);
		void signalAssemblyAdd(Assembly* assembly);

		void signalComponentMerge(Component* a, Component* b);
		void signalAssemblyMerge(Assembly* a, Assembly* b);

		// Equivalent to deleting object
		void signalAssemblyDelete(Assembly* assembly);
		void signalComponentDelete(Component* component);
	};
}
