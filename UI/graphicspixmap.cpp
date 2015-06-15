#include "graphicspixmap.h"
#include "graphicsitemgroup.h"

namespace CDI
{
	GraphicsPixmap::GraphicsPixmap(GraphicsItemGroup *parent, Image *image)
		:QGraphicsPixmapItem(parent)
	{
		parentImage = image;
		updateImage();
	}

	void GraphicsPixmap::updateImage()
	{
		if (parentImage== NULL) return;
		setOffset(0,0);
		setPixmap(parentImage->pixmap());
		setTransform(parentImage->transform());
	}
}
