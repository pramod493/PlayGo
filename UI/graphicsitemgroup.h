#pragma once

#include <QGraphicsItemGroup>
#include <QGraphicsPixmapItem>
#include "commonfunctions.h"
#include <QDebug>

namespace CDI
{
	class GraphicsItemGroup : public QGraphicsItemGroup
    {
    public:
        GraphicsItemGroup(QGraphicsItem* graphicsParent, QObject* objectParent);

        ~GraphicsItemGroup();

        bool sceneEvent(QEvent* event)
        {
            return QGraphicsItemGroup::sceneEvent(event);
        }

    };
}
