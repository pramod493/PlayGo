#pragma once
#include <QHash>
#include <QTransform>
#include "commonfunctions.h"
#include "abstractmodelitem.h"
#include "stroke.h"
#include "image.h"

namespace CDI
{
	class Page;
	// This class can contain Component objects as well
	// TODO - Destructor should delete all the items as well
	// Do not add Assembly objects to this.
	// Limit types to COMPONENT, STROKE, IMAGE, PHYSICS..., POLYGON2D
	class Component : public AbstractModelItem, public QHash<QUuid, AbstractModelItem*>
	{
		Q_OBJECT
	protected:
		Page* _pagePtr;
		QTransform _transform;

	protected:
		/**
		 * @brief Component : Creates Component object. Private constructor prevents
		 * creation by any other entities. Create and delete operation handled by Page
		 * or Assembly object (declared as friend to this class.
		 */
		Component(Page* parent);

		/**
		 * @brief ~Component handles deletion of Component object. It also manages the
		 * deletion of contained AbstractModelItem objects.
		 */
		virtual ~Component() {}
	public:
		QTransform transform() const;

		/**
		 * @brief Get transformation w.r.t. to given component
		 * @param itemId
		 * @return Absolute transform of given item.
		 */
		virtual QTransform itemTransform(AbstractModelItem*) const;

		/**
		 * @brief Get transformation w.r.t. to given component
		 * @param itemId:QUuid of the item
		 * @return Absolute transform of given item. Returns transform of
		 * first item in case of multiple matches
		 */
		virtual QTransform itemTransform(QUuid itemId) const;

		/**
		 * @brief setTransform sets the Component transform
		 * @param transform: New transformation
		 */
		virtual void setTransform(QTransform& transform, bool combine = false);

		// Extra create functions for convenience. Automatically adds the item
		// given component.
		virtual Stroke* addStroke(QColor color, float thickness);
		virtual Image* addImage(const QString filename);

		virtual void addItem(AbstractModelItem* item);
		virtual bool removeItem(AbstractModelItem* item);

		// Query functions.
		// Use these instead of default query functions of QHash fo extra features
		// NOTE: Avoid searchRecursive option unless required. Affect on performance
		// not measured.
		virtual bool containsItem(AbstractModelItem* key, bool searchRecursive = false);
		virtual bool containsItem(QUuid key, bool searchRecursive = false);
		virtual bool containsItem(QString key, bool searchRecursive = false);

		virtual AbstractModelItem* getItemPtrById(QUuid key, bool searchRecursive = false);

		ItemType type() const;

		QDataStream& serialize(QDataStream& stream) const;
		QDataStream& deserialize(QDataStream& stream);

	protected:
		// If new item types are added, extend this class to accomodate those
		virtual AbstractModelItem* createEmptyItem(ItemType itemtype);

		friend class Page;
		friend class Assembly;	// Try to make it work without allowing assembly to
		// create/delete components
	};
}
