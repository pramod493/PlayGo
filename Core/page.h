#pragma once
#include <QDataStream>
#include <QObject>
#include <QList>
#include <QUuid>

#include "commonfunctions.h"
#include "abstractmodelitem.h"
#include "assembly.h"
#include "component.h"

namespace CDI
{
	class PlayGo;
	/**
	 * @brief The Page class is the top level item which contains self sufficient information regarding a scene.
	 * It also serves the function of factory class to create objects
	 * This is the top-level container for all AbstractModelItem objects
	 */
	class Page : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(QUuid id READ id)

	protected:
		PlayGo *_playgo;

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

	public:
		Page(PlayGo* parent);

		virtual ~Page();

		// Deletes all the components and assemblies in the page
		void deleteAll();

		inline QUuid id() const { return _id; }

		/**
		 * @brief add copies the given page information into this page
		 * @param page: Page to add
		 * @return True on success, false on fail
		 */
		bool add(Page* page);

		Assembly* createAssembly();
		Component* createComponent();

		QList<Assembly*> getAssemblies() const;
		QList<Component*> getComponents() const;

		// Merge a2 into a1
		bool mergeAssembly(Assembly* a1, Assembly* a2);

		bool destroyAssembly(Assembly* assembly);
		bool destroyComponent(Component* component);

		void triggerUpdate(AbstractModelItem* item);

		/**
		 * @brief getParent returns the immediate parent Component or
		 * Assembly of a given AbstractModelItem object
		 * In case no such parent is present, NULL is returned.
		 * In case the item is not present in the
		 * @param item
		 * @return parent object of the item
		 */
		AbstractModelItem* getParent(QUuid id);

		AbstractModelItem* getItemPtrById(QUuid id);

		AbstractModelItem* getParent(AbstractModelItem* item);

		bool contains(QUuid id, bool searchRecursive = false);

		QDataStream& serialize(QDataStream &stream) const;
		QDataStream& deserialize(QDataStream &stream);

		friend QDataStream& operator<<(QDataStream& stream, const Page& page);
		friend QDataStream& operator>>(QDataStream& stream, Page& page);
	signals:
		void afterComponentAddition(Component* component);
		void afterAssemblyAdditton(Assembly* assembly);

		void beforeComponentDeletion(Component* component);
		void beforeAssemblyDeletion(Assembly* assembly);

		void onComponentMerge(Component* a, Component* b);
		void onAssemblyMerge(Assembly* a, Assembly* b);

		void onAssemblyUpdate(Assembly* assembly);
		void onComponentUpdate(Component* component);

		void onPageUpdate(Page* page);	// triggers reloading of whole page information
		void onPageLoad(Page* page);
	};
}
