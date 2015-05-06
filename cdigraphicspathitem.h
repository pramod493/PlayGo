#pragma once

#include <QGraphicsPathItem>
#include <QPointF>
#include <QPainterPath>
#include <QObject>
#include "stroke.h"
#include <vector>

namespace CDI
{
    class GraphicsPathItem :public QObject, public QGraphicsPathItem
    {
        Q_OBJECT
    public:

        Stroke* parentStroke;

		QPainterPath painterPath;

		bool isOrphan;		// If there is  no parent. True as default

        GraphicsPathItem(QGraphicsItem* parent, QPointF startPoint);

        GraphicsPathItem(QGraphicsItem* parent, Stroke* stroke);

        ~GraphicsPathItem();

		void push_back(QPointF point);

		void push_back(Point2DPT point);

        void ApplySmoothing(int order);

		bool Selected(QPointF point, float extraWidth);

    public slots:
        void OnStrokeUpdate(Stroke* stroke);
    };
}
