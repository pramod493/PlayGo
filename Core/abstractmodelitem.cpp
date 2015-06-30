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
	{
		itemId = uniqueHash();
		mask = 0;
		_parentComponent = NULL;	// Do not set to QObject here
	}

	AbstractModelItem::~AbstractModelItem()
	{
		if (_parentComponent!= NULL)
		{
            _parentComponent->onItemDelete(this);
		}
	}

	QUuid AbstractModelItem::id() const
	{
		return itemId;
	}

	QTransform AbstractModelItem::globalTransform() const
	{
		if (parentItem()== NULL)
			return transform();
		else
			return parentItem()->globalTransform() * transform();
	}

	Component* AbstractModelItem::parentItem() const
	{
		return _parentComponent;
	}

	bool AbstractModelItem::setParentItem(Component *parentComponent)
	{
		// Prevent infinite loop in case of bad prog.
		if (_parentComponent == parentComponent) return false;
		if (_parentComponent!= NULL) _parentComponent->removeItem(this);

		_parentComponent = parentComponent;
		if (_parentComponent != NULL) _parentComponent->addItem(this);

		return true;
	}

	QTransform AbstractModelItem::inverseTransform() const
	{
		return transform().inverted();
	}

	void AbstractModelItem::setVisible(bool visible)
	{
        if(mask & isLocked)  return;
		if (visible == isVisible()) return;	// No need to change the state of display
		if (visible) mask &= ~isHidden;
		else mask |= isHidden;
		if(_parentComponent!= NULL)
			_parentComponent->onItemDisplayUpdate(id());
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

    bool AbstractModelItem::locked() const
    {
        if (mask & isLocked) return true;
        return false;
    }

    void AbstractModelItem::lock(bool value)
    {
        if (value)
            mask |= isLocked;
        else
            mask &= ~isLocked;
    }

	QDataStream& operator<<(QDataStream& stream, const AbstractModelItem& item)
	{
		return item.serialize(stream);
	}

	QDataStream& operator>>(QDataStream& stream, AbstractModelItem& item)
	{
		return item.serialize(stream);
	}

	AbstractModelItem* getItemPtrByType(ItemType t, Component *parentComponent)
	{
		// NOTE - In case int is available, use getItemType(int i)
		// to get the ItemType

		AbstractModelItem* ptr = NULL;

		switch (t)
		{
		case STROKE :
			ptr = parentComponent->addStroke();
			break;
		case IMAGE :
			ptr = parentComponent->addImage();
			break;
		case POLYGON2D :
			ptr = new Polygon2D(parentComponent);
			parentComponent->addItem(ptr);
//		case COMPONENT :
//			ptr = new Component();
//		case ASSEMBLY :
//			ptr = new Assembly();
		default :
			qDebug() << "Default constructor for given itemtype is not available. Returning NULL";
		}
		return ptr;
	}

}
