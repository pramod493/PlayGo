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
        enum {Type = UserType + IMAGEVIEW };

    protected:
        Image* _parentImage;

        bool _highlighted;

    public:
		GraphicsPixmap(GraphicsItemGroup* parent, Image* image);

		int type() const { return Type; }

        Image* parentImage() const;

        void setParentImage(Image* image);

        bool contains(QPointF pos);

		void updateImage();

        bool isHighlighted() const;

        void highlight(bool value);
	};
}
