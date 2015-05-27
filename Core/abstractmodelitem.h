#pragma once
#include "commonfunctions.h"
#include <QDataStream>
#include <QUuid>

namespace CDI {
	class AbstractModelItem
	{
	public:
		unsigned int mask;	/**< Item's mask */
	protected:
		QUuid itemId;		/**< Unique item ID */

	public:
		inline AbstractModelItem() {itemId = uniqueHash(); mask = 0;}
		virtual ~AbstractModelItem() {}

		inline QUuid id() const;

		virtual ItemType type() const = 0;
		virtual QTransform transform() const { return QTransform(); }
		/**
		 * @brief inverseTransform returns the inverse of transform returned by transform()
		 * Inverse is calculated for every step. Avoid unless necessary
		 * Derived classes might re-implement this function for faster processing
		 * @return Inverse Transformation of object
		 */
		virtual QTransform inverseTransform() const {return transform().inverted();}

		virtual QDataStream& serialize(QDataStream& stream) const = 0;
		virtual QDataStream& deserialize(QDataStream& stream) = 0;

		friend QDataStream& operator<<(QDataStream& stream, const AbstractModelItem& item);
		friend QDataStream& operator>>(QDataStream& stream, AbstractModelItem& item);

	private:
		QDataStream& serializeInternal(QDataStream& stream) const;
		QDataStream& deserializeInternal(QDataStream& stream);

	};

	inline QUuid AbstractModelItem::id() const
	{
		return itemId;
	}

	AbstractModelItem* getItemPtrByType(ItemType t);
}
