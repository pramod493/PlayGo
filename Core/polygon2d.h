#pragma once

#include <QGraphicsPolygonItem>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>
#include "commonfunctions.h"
#include "ramerdouglaspeucker.h"
#include "cdi2qtwrapper.h"

namespace CDI
{
	class Polygon2D : public QGraphicsPolygonItem
	{
	public:
        enum {Type = UserType+ POLYGONVIEW};

	protected:
		bool _highlighted;
		QUuid _id;

		bool applyCustomRenderSettings;

	public:
		Polygon2D(QGraphicsItem* parent = 0);

		Polygon2D(const QPolygonF& polygon, QGraphicsItem* parent = 0);

		virtual ~Polygon2D();

		int type() const { return Type; }

		QUuid id() const { return _id; }

		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

		void applySmoothing(float margin);

		virtual bool containsPoint(const Point2D &pt, SelectionType rule, float margin=0);
	};
}
