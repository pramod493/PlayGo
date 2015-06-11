#include <QString>
#include "abstractmodelitem.h"
#include "stroke.h"
#include "image.h"
#include "polygon2d.h"
#include "component.h"
#include "assembly.h"

namespace CDI
{
	AbstractModelItem::AbstractModelItem()
		: QObject(NULL)
	{
		itemId = uniqueHash();
		mask = 0;
		_parent = NULL;	// Do not set to QObject here
	}

	AbstractModelItem::AbstractModelItem(QObject *parent)
		: QObject(parent)
	{
		itemId = uniqueHash();
		mask = 0;
		_parent = NULL;	// Do not set to QObject here
	}

	QUuid AbstractModelItem::id() const
	{
		return itemId;
	}

	QTransform AbstractModelItem::transform() const
	{
		return QTransform();
	}

	QTransform AbstractModelItem::globalTransform() const
	{
		if (parentItem()== NULL)
			return transform();
		else
			return parentItem()->transform() * transform();
	}

	AbstractModelItem* AbstractModelItem::parentItem() const
	{
		return _parent;
	}

	void AbstractModelItem::setParentItem(AbstractModelItem *item)
	{
		// TODO - Update transformation
		_parent = item;
		setParent(item);
		emit parentObjectChanged(this);
	}

	void AbstractModelItem::setVisible(bool visible)
	{
		if (visible) mask &= ~isHidden;
		else mask |= isHidden;
		emit displayStatusChanged(this);
	}

	bool AbstractModelItem::isVisible() const
	{
		if (mask & isHidden) return false;
		else return true;
	}

	void AbstractModelItem::show()
	{
		setVisible(true);
	}

	void AbstractModelItem::hide()
	{
		setVisible(false);
	}

	QDataStream& AbstractModelItem::serializeInternal(QDataStream& stream) const
	{
		stream << itemId;
		return this->serialize(stream);
	}

	QDataStream& AbstractModelItem::deserializeInternal(QDataStream& stream)
	{
		stream >> itemId;
		return this->deserialize(stream);
	}

	QDataStream& operator<<(QDataStream& stream, const AbstractModelItem& item)
	{
		return item.serializeInternal(stream);
	}

	QDataStream& operator>>(QDataStream& stream, AbstractModelItem& item)
	{
		return item.deserializeInternal(stream);
	}

	AbstractModelItem* getItemPtrByType(ItemType t)
	{
		// NOTE - In case int is available, use getItemType(int i)
		// to get the ItemType

		AbstractModelItem* ptr = NULL;

		switch (t)
		{
		case ItemType::STROKE :
			ptr = new Stroke();
			break;
		case ItemType::IMAGE :
			ptr = new Image();
			break;
		case ItemType::POLYGON2D :
			ptr = new Polygon2D();
//		case ItemType::COMPONENT :
//			ptr = new Component();
//		case ItemType::ASSEMBLY :
//			ptr = new Assembly();
		default :
			qDebug() << "Default constructor for given itemtype is not available. Returning NULL";
		}
		return ptr;
	}

}
