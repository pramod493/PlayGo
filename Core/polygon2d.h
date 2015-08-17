#pragma once

#include <QGraphicsPolygonItem>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>
#include "commonfunctions.h"
#include "ramerdouglaspeucker.h"
#include "cdi2qtwrapper.h"
#include "physicsshape.h"

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
		QPolygonF _internalPolygon;
		QPolygonF _simplifiedPolygon;

		PhysicsShape* _physicsShape;

	public:
		Polygon2D(QGraphicsItem* parent = 0);

		Polygon2D(const QPolygonF& polygon, QGraphicsItem* parent = 0);

		Polygon2D(const Polygon2D& copy);

		virtual ~Polygon2D();

		int type() const { return Type; }

		QUuid id() const { return _id; }

		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

		virtual void push_point(QPointF pos);

		PhysicsShape *physicsShape() const;

		static QPolygonF applyRDPSmoothing(QPolygonF poly, float margin);

		static QPolygonF applyLaplacianSmoothing(QPolygonF poly, int order);

		virtual bool containsPoint(const Point2D &pt, SelectionType rule = Inside, float margin=0);

		bool isHighlighted() const;

		void highlight(bool value);

		void initializePhysicsShape();

		virtual QDataStream& serialize(QDataStream& stream) const;

		virtual QDataStream& deserialize(QDataStream& stream);

	};
}
