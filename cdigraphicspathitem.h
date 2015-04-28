#pragma once

#include <QGraphicsPathItem>
#include <QPointF>
#include <QPainterPath>
#include "stroke.h"
#include <vector>

namespace CDI
{
    class CDIGraphicsPathItem : public QGraphicsPathItem
    {
    public:

        Stroke* parentStroke;

		QPainterPath painterPath;

		bool isOrphan;		// If there is  no parent. True as default

		CDIGraphicsPathItem(QGraphicsItem* parent, QPointF startPoint);

        ~CDIGraphicsPathItem();

		void push_back(QPointF point);

		void push_back(Point2DPT point);

        void ApplySmoothing(int order);
    };
}
