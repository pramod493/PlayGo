#include "pixmap.h"

namespace CDI
{
	Pixmap::Pixmap(QGraphicsItem *parent)
		:QGraphicsPixmapItem(parent)
	{
		_id = uniqueHash();
        _highlighted = false;
        setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
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
	}

	Pixmap::Pixmap(const QPixmap &pixmap, QString filepath, QGraphicsItem* parent)
		: QGraphicsPixmapItem(pixmap, parent)
	{
		_id = uniqueHash();
		_highlighted = false;

		_filename = filepath;
		setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
	}

	void Pixmap::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
	{
#ifdef CDI_DEBUG_DRAW_SHAPE
		painter->drawRect(boundingRect());
#endif
		QGraphicsPixmapItem::paint(painter, option, widget);
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
}
