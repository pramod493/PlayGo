#pragma once

#include <QGraphicsItemGroup>
#include <QGraphicsPixmapItem>

#include "commonfunctions.h"
#include "abstractmodelitem.h"
#include "component.h"

#include "cdi2qtwrapper.h"

#include "graphicspathitem.h"
#include "graphicspolygon2d.h"

#include <QDebug>

namespace CDI
{
	class SketchScene;

	/**
	 * @brief It is equivalent of Component in visualization size. This stores references to the all the children
	 * Also, note that all the children take care of their transform and therefore need not implement the
	 * boundingRect() function in this as well as component class.
	 */
	class GraphicsItemGroup : public QGraphicsItemGroup
    {
	public:
		Component* component;
    public:
		GraphicsItemGroup(QGraphicsItem* graphicsParent = NULL);

        ~GraphicsItemGroup();

		int type () const { return QGraphicsItem::UserType + GraphicsItemType::COMPONENTVIEW; }

		friend class SketchScene;
    };
}
