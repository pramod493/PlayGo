#pragma once

#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>
#include "polygon2d.h"
#include "point2dpt.h"
#include "commonfunctions.h"

#include "cdi2qtwrapper.h"

namespace CDI
{
	class GraphicsItemGroup;

	class GraphicsPolygon2D : public QGraphicsPathItem
	{
	public:
		Polygon2D* parentPolygon;

        enum {Type = UserType+ POLYGONVIEW};

	public:
//		GraphicsPolygon2D(QGraphicsItem* parent, Point2D startPoint, float pressure=1.0f, int time = 0);

		GraphicsPolygon2D(GraphicsItemGroup* parent, Polygon2D* polygon);

		virtual ~GraphicsPolygon2D();

		QRectF boundingRect() const;

		int type() const { return Type; }

		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

		void push_back(QPointF point);

		void push_back(Point2DPT point);

		void ApplySmoothing(float margin);

		bool containsPoint(const Point2D &pt, SelectionType rule, float margin=0);

		bool Selected(QPointF point, float extraWidth);

		void updatePolygon();
	};
}
