#pragma once

#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>
#include "stroke.h"
#include "point2dpt.h"
#include "commonfunctions.h"

namespace CDI
{
	class GraphicsPathItem : public QGraphicsPathItem
	{
	public:

		Stroke* parentStroke;

//		bool isOrphan;		// If there is  no parent. True as default

	public:
		GraphicsPathItem(QGraphicsItem* parent, Point2D startPoint, float pressure=1.0f, int time = 0);

		GraphicsPathItem(QGraphicsItem* parent, Stroke* stroke);

		~GraphicsPathItem();

		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

		void push_back(QPointF point, float pressure=1.0, int time = 0);

		void push_back(Point2DPT point);

		void ApplySmoothing(int order);

		bool Selected(QPointF point, float extraWidth);

		Q_DECL_DEPRECATED void OnStrokeUpdate(Stroke* stroke);
	};
}
