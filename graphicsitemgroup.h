#pragma once

#include <QGraphicsItemGroup>
#include <QGraphicsPixmapItem>
#include "cdicommon.h"
#include <QDebug>

namespace CDI
{
    class GraphicsItemGroup : public QGraphicsItemGroup, public Item
    {
    public:
        GraphicsItemGroup(QGraphicsItem* graphicsParent, QObject* objectParent);

        ~GraphicsItemGroup();

        bool sceneEvent(QEvent* event)
        {
            return QGraphicsItemGroup::sceneEvent(event);
        }

        Item::Type GetType() { return Type::COMPOUND_SHAPE; }
    };
}
