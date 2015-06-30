#include "graphicspixmap.h"
#include "graphicsitemgroup.h"

namespace CDI
{
	GraphicsPixmap::GraphicsPixmap(GraphicsItemGroup *parent, Image *image)
		:QGraphicsPixmapItem(parent)
	{
        _parentImage = image;
        _highlighted = false;
        // Use the bounding box for all purposes
        setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
		updateImage();
    }

    Image* GraphicsPixmap::parentImage() const
    {
        return _parentImage;
    }

    void GraphicsPixmap::setParentImage(Image *image)
    {
        _parentImage = image;
    }

    bool GraphicsPixmap::contains(QPointF pos)
    {
        return false;
    }

	void GraphicsPixmap::updateImage()
	{
        if (_parentImage== NULL) return;
        setOffset(0,0);
        QPixmap* tmp = _parentImage->pixmap();
        setPixmap(*tmp);
        setTransform(_parentImage->transform());
	}

    bool GraphicsPixmap::isHighlighted() const
    {
        return _highlighted;
    }

    void GraphicsPixmap::highlight(bool value)
    {
        if (_highlighted == value) return;  // Do nothing
        _highlighted = value;
    }
}
