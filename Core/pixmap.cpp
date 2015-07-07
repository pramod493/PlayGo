#include "pixmap.h"

namespace CDI
{
	Pixmap::Pixmap(QGraphicsItem *parent)
		:QGraphicsPixmapItem(parent)
	{
		_id = uniqueHash();
        _highlighted = false;
        setShapeMode(QGraphicsPixmapItem::BoundingRectShape);

		initializePhysicsShape();
    }

	Pixmap::Pixmap(QString filepath, QGraphicsItem* parent)
		: QGraphicsPixmapItem(parent)
	{
		_id = uniqueHash();
		_highlighted = false;

		_filename = filepath;
		QFile f(_filename);
		if (f.exists())
		{
			QPixmap p = QPixmap();
			p.load(_filename);
			setPixmap(p);
		}

		setShapeMode(QGraphicsPixmapItem::BoundingRectShape);

		initializePhysicsShape();
	}

	Pixmap::Pixmap(const QPixmap &pixmap, QString filepath, QGraphicsItem* parent)
		: QGraphicsPixmapItem(pixmap, parent)
	{
		_id = uniqueHash();
		_highlighted = false;

		_filename = filepath;
		setShapeMode(QGraphicsPixmapItem::BoundingRectShape);

		initializePhysicsShape();
	}

	void Pixmap::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
	{
#ifdef CDI_DEBUG_DRAW_SHAPE
		painter->drawRect(boundingRect());
#endif
		QGraphicsPixmapItem::paint(painter, option, widget);
	}

	PhysicsShape* Pixmap::physicsShape() const
	{
		return _physicsShape;
	}

	void Pixmap::resetPhysicsShape()
	{
		if (_physicsShape) delete _physicsShape;

	}

	bool Pixmap::isHighlighted() const
    {
        return _highlighted;
    }

	void Pixmap::highlight(bool value)
    {
        if (_highlighted == value) return;  // Do nothing
        _highlighted = value;
    }

	QDataStream& Pixmap::serialize(QDataStream& stream) const
	{
		stream << _id;
		return stream;
	}

	QDataStream& Pixmap::deserialize(QDataStream& stream)
	{
		stream >> _id;
		return stream;
	}

	void Pixmap::initializePhysicsShape()
	{
		_physicsShape == NULL;
	}
}
