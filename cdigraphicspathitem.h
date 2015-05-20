#pragma once

#include <QGraphicsPathItem>
#include <QPointF>
#include <QPainterPath>
#include <QObject>
#include "stroke.h"
#include <vector>
#include <QStyleOptionGraphicsItem>
#include <Box2D/Dynamics/b2Body.h>

namespace CDI
{
    class GraphicsPathItem :public QObject, public QGraphicsPathItem
    {
        Q_OBJECT
    public:
        b2Body* physicsBody;

        float physicsMultiplier;
        float physicsDivider;

        Stroke* parentStroke;

		QPainterPath painterPath;

		bool isOrphan;		// If there is  no parent. True as default

        GraphicsPathItem(QGraphicsItem* parent, QPointF startPoint, float pressure=1.0, long time = 0);

        GraphicsPathItem(QGraphicsItem* parent, Stroke* stroke);

        ~GraphicsPathItem();

        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

        void push_back(QPointF point, float pressure=1.0, long time = 0);

		void push_back(Point2DPT point);

        void ApplySmoothing(int order);

		bool Selected(QPointF point, float extraWidth);

    public slots:
        void OnStrokeUpdate(Stroke* stroke);
    };
}
