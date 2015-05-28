#pragma once

#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>
#include "polygon2d.h"
#include "point2dpt.h"
#include "commonfunctions.h"

namespace CDI
{
	class GraphicsPolygon2D : public QGraphicsPathItem
	{
	public:

		Polygon2D* parentPolygon;

//		bool isOrphan;		// If there is  no parent. True as default

	public:
		GraphicsPolygon2D(QGraphicsItem* parent, Point2D startPoint, float pressure=1.0f, int time = 0);

		GraphicsPolygon2D(QGraphicsItem* parent, Polygon2D* polygon);

		~GraphicsPolygon2D();

		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

		void push_back(QPointF point);

		void push_back(Point2DPT point);

		void ApplySmoothing(float margin);

		bool containsPoint(const Point2D &pt, SelectionType rule, float margin=0) {return false;}

		bool Selected(QPointF point, float extraWidth);
	};
}
