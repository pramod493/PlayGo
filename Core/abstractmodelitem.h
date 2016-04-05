#pragma once
#include "commonfunctions.h"
#include <QDataStream>
#include <QUuid>
#include <QObject>

namespace CDI {

	class Component;

	/**
	 * @brief The Item class is base class for all the components derived in the Model
	 * @remarks - Can we also have item type for manager classes?. Also see
	 * http://stackoverflow.com/questions/318064/how-do-you-declare-an-interface-in-c
	 * NOTE - Manager classes might might have settings that we need to store
	 */
	class Item
	{
	public:
		virtual ~Item() {}
		/**
		 * @brief type return the AbstractModelItem type. Must be implemented by
		 * derived classes
		 * @return Item Type
		 */
		virtual ItemType type() const = 0;

		/**
		 * @brief id returns the item ID
		 * @return item's QUuid
		 */
		virtual QUuid id() const = 0;

		/**
		 * @brief serialize implements the serialization of the object
		 * @param stream QDataStream on which to write the information
		 * @return QDataStream after write
		 */
		virtual QDataStream& serialize(QDataStream& stream) const = 0;

		/**
		 * @brief deserialize implements deserialization of the object
		 * @param stream QDataStream from which to read the information
		 * @return  QDataStream after writing
		 */
		virtual QDataStream& deserialize(QDataStream& stream) = 0;
	};
}
