#pragma once
#include "commonfunctions.h"
#include <QDataStream>
#include <QUuid>
#include <QObject>

namespace CDI {

	class Component;

	/**
	 * @brief The Item class is base class for all the components derived in the Model
	 * @remarks - Can we also have item type for manager classes?
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

	/**
	 * @brief The AbstractModelItem class is base class for all the items created in model.
	 *
	 */
	class AbstractModelItem : public Item
	{
	public:
		unsigned int mask;	/**< Item's mask */

	protected:
		QUuid itemId;		/**< Unique item ID */
		Component* _parentComponent;

	public:
		//-----------------------------------------------
		// Constructors/Destructors
		//-----------------------------------------------
		AbstractModelItem();

		virtual ~AbstractModelItem();

		//-----------------------------------------------
		// Query/Set functions(same order in derived class)
		// Non-virtual
		//-----------------------------------------------
		/**
		 * @brief id returns the item ID
		 * @return item's QUuid
		 */
		QUuid id() const;

		//-----------------------------------------------
		// Virtual functions (same order in derived class)
		//-----------------------------------------------

		/**
		 * @brief transform returns the item's transformation w.r.t it's parent
		 * @return Local transformation
		 */
		virtual QTransform transform() const = 0;

		/**
		 * @brief Sets the Item transform. Override this function to enable
		 * different behavior such as propagating transformatin to component children
		 * @param t New item transform
		 */
		virtual void setTransform(QTransform t) = 0;

		/**
		 * @brief globalTransform returns the global transformation of the item
		 * @return Global transformation
		 */
		virtual QTransform globalTransform() const;

		/**
		 * @brief parentItem returns the parent of an item
		 * @return Pointer to parent item
		 */
		virtual Component* parentItem() const;

		/**
		 * @brief setParentItem sets the parent item value
		 * @param item
		 * @return True if the given component is correctly set as parent
		 * \todo Check if the transformation needs to be updated following a change of parent item
		 * For Item== NULL, the transformation is an identity matrix
		 */
		virtual bool setParentItem(Component* parentComponent);

		/**
		 * @brief inverseTransform returns the inverse of transform returned by transform()
		 * Inverse is calculated for every step. Avoid unless necessary
		 * Derived classes might re-implement this function for faster processing
		 * @return Inverse Transformation of object
		 */
		virtual QTransform inverseTransform() const;

		/**
		 * @brief setVisible sets visibility of the object
		 * @param visible Display state of object
		 */
		virtual void setVisible(bool visible);
		/**
		 * @brief isVisible
		 * @return Display state of the object
		 */
		virtual bool isVisible() const;
		/**
		 * @brief show sets the display state of the object as visible
		 */
		virtual void show();
		/**
		 * @brief hide sets the display state of object as hidden
		 */
		virtual void hide();

        virtual bool locked() const;

        virtual void lock(bool value);

	private:
//		/**
//		 * @brief serializeInternal internal function which writes the QUuid of the item
//		 * @param stream
//		 * @return
//		 */
//		QDataStream& serializeInternal(QDataStream& stream) const;
//		/**
//		 * @brief deserializeInternal private function which reads back the QUuid of the item
//		 * @param stream
//		 * @return
//		 */
//		QDataStream& deserializeInternal(QDataStream& stream);

//	signals:
//		void itemDelete(AbstractModelItem *item);
//		void transformUpdate(AbstractModelItem* item);
//		void displayUpdate(AbstractModelItem* item);

		//-----------------------------------------------
		// Friend functions and classes
		//-----------------------------------------------
		/**
		 * @brief operator << Overload operator for QDataStream serialization
		 * @param stream
		 * @param item
		 * @return QDataStream reference after writing
		 */
		friend QDataStream& operator<<(QDataStream& stream, const AbstractModelItem& item);
		friend QDataStream& operator>>(QDataStream& stream, AbstractModelItem& item);


	};

	//-----------------------------------------------
	// Pther functions related to the class
	//-----------------------------------------------
	AbstractModelItem* getItemPtrByType(ItemType t, Component* parentComponent);
}
