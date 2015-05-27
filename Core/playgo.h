#pragma once
#include <QHash>
#include "commonfunctions.h"
#include "abstractmodelitem.h"
#include "component.h"
#include "assembly.h"

/**
 \mainpage PlayGo core component documentation
 \section Introducion to datastructure
 All objetcs are derived from AbstractModelItem object
 \section Conact Info
Author: Pramod Kumar
email: pramod@purdue.edu
email: pramod493@gmail.com
 */
namespace CDI
{
	/**
	 * @brief
	 * The root object which contains all the components.
	 * Mostly used for loading/saving files and maintaining ID unique
	 */
	class PlayGo
	{
	protected:
		QHash<QUuid, Assembly*> assemblies;
		QHash<QUuid, Component*> freeComponents;
	public:
		PlayGo();

		/**
		 * @brief Delete all the components in the scene and destroys the object
		 */
		~PlayGo();

		/**
		 * @brief deleteAllItems: Clear all items in the database
		 */
		void deleteAllItems();

		virtual Component* addComponent(Assembly* parent);
		virtual Assembly* addAssembly();

		virtual bool contains(Assembly* assembly);
		virtual bool contains(QUuid id, bool searchRecursive);
		virtual bool contains(QString id, bool searchRecursive);

		virtual Assembly* getAssemblyById(QUuid id);
		virtual Assembly* getAssemblyById(QString id);

		QDataStream& serialize(QDataStream& stream) const;
		QDataStream& deserialize(QDataStream& stream);

		friend QDataStream& operator<<(QDataStream& stream, const PlayGo& item);
		friend QDataStream& operator>>(QDataStream& stream, PlayGo& item);
	};
}
