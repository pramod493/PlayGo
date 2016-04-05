#pragma once

#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QVector>
#include "cdi2qtwrapper.h"
#include "point2dpt.h"

namespace CDI
{
	class Stroke : public QGraphicsPathItem
	{
	public:
		enum { Type = UserType + STROKEVIEW };

	protected:
		bool _highlighted;
		QUuid _id;
		QVector<Point2DPT*> _points;

		// For bounding box calculations
		float _x_min, _x_max, _y_min, _y_max;
		QRectF aabb;
		bool _isStrokeFinalized;
	public:

		/**
		 * @brief Create a new GraphicsPathItem object based on Stroke object
		 * @param parent parent QGraphicsItem object
		 * @param stroke Stroke object
		 */
		Stroke(QGraphicsItem* parent = 0);

		Stroke(QVector<Point2DPT*> points, QGraphicsItem* parent = 0);

		virtual ~Stroke();

		int type() const { return Type; }

		QUuid id() const { return _id; }

		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

		int size() { return _points.size(); }

		/**
		 * @brief setStrokePath sets the given vector as current stroke
		 * @param points vector of points
		 */
		void setStrokePath(QVector<Point2DPT*> points);

		bool contains(const QPointF& point) const;

		virtual bool contains(const QPointF& point, float margin) const;

		virtual bool isContainedWithin(QPolygonF* polygon, float percentmatch = 1.0f);

		QRectF boundingRect() const;

		void push_point(QPointF point, float pressure=1.0, int time = 0);

		void push_point(Point2DPT point);

		void applySmoothing(int order);

		bool isHighlighted() const;

		void highlight(bool value);

		QDataStream& serialize(QDataStream& stream) const;

		QDataStream& deserialize(QDataStream& stream);

	protected:
		virtual void init();

		virtual void updateAABB(float x, float y);

		virtual void recalculateAABB();
	};
}
