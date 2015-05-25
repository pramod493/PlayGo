#pragma once
#include <QHash>
#include <QTransform>
#include "commonfunctions.h"
#include "abstractmodelitem.h"
#include "stroke.h"
#include "image.h"


namespace CDI
{
	// This class can contain Component objects as well
	class Component : public QHash<QString, AbstractModelItem*>, public AbstractModelItem
	{
	protected:
		QTransform _transform;

	public:
		Component();

		QTransform transform() const;
		QTransform itemTransform(AbstractModelItem*) const;
		QTransform itemTransform(QString itemId) const;

		inline void setTransform(QTransform& transform);

		// These are overload functions and items can be added/removed directly as well
		inline void addItem(AbstractModelItem* item);
		inline bool removeItem(AbstractModelItem* item);

		// These operations should be preferred over directly adding components
		inline Stroke* addStroke(QColor color, float thickness);
		inline Image* addImage(const QString filename);

		// virtual functions
		ItemType type() const;

		QDataStream& serialize(QDataStream& stream) const;
		QDataStream& deserialize(QDataStream& stream);

	protected:
		// If new item types are added, extend this class to accomodate those
		virtual AbstractModelItem* createEmptyItem(ItemType itemtype);
	};

	inline QTransform Component::transform() const
	{
		return _transform;
	}

	inline void Component::setTransform(QTransform& transform)
	{
		mask |= isTransformed;
		_transform = transform;
	}


	inline void Component::addItem(AbstractModelItem* item)
	{
		insert(item->id().toString(), item);
	}

	inline bool Component::removeItem(AbstractModelItem* item)
	{
		return ((remove(item->id().toString())!=0) ? true : false);
	}

	inline Stroke* Component::addStroke(QColor color, float thickness)
	{
		Stroke* stroke = new Stroke(color, thickness);
		addItem(stroke);
		return stroke;
	}

	inline Image* Component::addImage(const QString filename)
	{
		Image* image = new Image(filename);
		addItem(image);
		return image;
	}
}
