#pragma once

#include <QGraphicsPathItem>
#include <QPointF>
#include <QPainterPath>
#include "stroke.h"

namespace CDI
{
    class CDIGraphicsPathItem : public QGraphicsPathItem
    {
    public:

        Stroke* parentStroke;



        bool isOrphan;

        CDIGraphicsPathItem(QGraphicsItem* parent = 0, Stroke* stroke = NULL);

        ~CDIGraphicsPathItem();

        void push_back(QPointF* point);

        void push_back(Point2DPT* point);
    };
}
