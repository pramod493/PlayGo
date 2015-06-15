#pragma once
#include <QGraphicsPixmapItem>
#include "image.h"
#include "cdi2qtwrapper.h"

namespace CDI
{
	class GraphicsItemGroup;

	class GraphicsPixmap : public QGraphicsPixmapItem
	{
	public:
		Image* parentImage;

		enum {Type = UserType + GraphicsItemType::IMAGEVIEW };

		GraphicsPixmap(GraphicsItemGroup* parent, Image* image);

		int type() const { return Type; }

		void updateImage();
	};
}
