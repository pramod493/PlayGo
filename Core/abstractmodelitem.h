#pragma once
#include "commonfunctions.h"
#include <QDataStream>
#include <QUuid>
#include <QObject>

namespace CDI {
	/**
	 * @brief The AbstractModelItem class is base class for all the items created in model.
	 *
	 */
	class AbstractModelItem : public QObject
	{
		Q_OBJECT
		Q_PROPERTY(bool Visible READ isVisible  WRITE setVisible)
		Q_PROPERTY(QUuid ID READ id)
		Q_PROPERTY(int mask MEMBER mask)

	public:
		unsigned int mask;	/**< Item's mask */
	protected:
		QUuid itemId;		/**< Unique item ID */
		AbstractModelItem* _parent;

	public:
		AbstractModelItem();

		AbstractModelItem(QObject *parent);

		virtual ~AbstractModelItem()
		{
			emit itemDestroyed(this);
		}

		/**
		 * @brief id returns the item ID
		 * @return item's QUuid
		 */
		QUuid id() const;

		/**
		 * @brief type return the AbstractModelItem type. Must be implemented by
		 * derived classes
		 * @return Item Type
		 */
		virtual ItemType type() const = 0;

		/**
		 * @brief transform returns the item's transformation w.r.t it's parent
		 * @return Local transformation
		 */
		virtual QTransform transform() const;

		/**
		 * @brief globalTransform returns the global transformation of the item
		 * @return Global transformation
		 */
		virtual QTransform globalTransform() const;

		/**
		 * @brief parentItem returns the parent of an item
		 * @return Pointer to parent item
		 */
		virtual AbstractModelItem* parentItem() const;

		/**
		 * @brief setParentItem sets the parent item value
		 * @param item
		 * \todo Check if the transformation needs to be updated following a change of parent item
		 * For Item== NULL, the transformation is an identity matrix
		 */
		virtual void setParentItem(AbstractModelItem* item);
		/**
		 * @brief inverseTransform returns the inverse of transform returned by transform()
		 * Inverse is calculated for every step. Avoid unless necessary
		 * Derived classes might re-implement this function for faster processing
		 * @return Inverse Transformation of object
		 */
		virtual QTransform inverseTransform() const {return transform().inverted();}

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

		/**
		 * @brief operator << Overload operator for QDataStream serialization
		 * @param stream
		 * @param item
		 * @return
		 */
		friend QDataStream& operator<<(QDataStream& stream, const AbstractModelItem& item);
		friend QDataStream& operator>>(QDataStream& stream, AbstractModelItem& item);

	private:
		/**
		 * @brief serializeInternal internal function which writes the QUuid of the item
		 * @param stream
		 * @return
		 */
		QDataStream& serializeInternal(QDataStream& stream) const;
		/**
		 * @brief deserializeInternal private function which reads back the QUuid of the item
		 * @param stream
		 * @return
		 */
		QDataStream& deserializeInternal(QDataStream& stream);

	signals:
		void itemDestroyed(AbstractModelItem *item);
		void transformChanged(AbstractModelItem *item);
		void displayStatusChanged(AbstractModelItem *item);
		void parentObjectChanged(AbstractModelItem *item);
	};

	AbstractModelItem* getItemPtrByType(ItemType t);
}
