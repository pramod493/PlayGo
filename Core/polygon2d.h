#pragma once
#include <commonfunctions.h>
#include <abstractmodelitem.h>
#include <QDataStream>
#include <point2dpt.h>
#include <QVector>
#include <QColor>
#include <QPolygonF>	// Use these APIs
#include <QRectF>

namespace CDI
{
	class Polygon2D : public QPolygonF, public AbstractModelItem
	{
	protected:
		QColor _color;				/**< Polygon2D color */
		float _thickness;			/**< Maximum thickness of Polygon2D */
		QTransform _transform;		/**< Current Polygon2D transform w.r.t. its parent */
		QTransform _inverseTransform;
		bool _isConvex;
	public:
		inline Polygon2D();
		inline Polygon2D(QColor color, float thickness);
		inline Polygon2D(const Polygon2D& s);
		inline Polygon2D(const QVector<Point2D>& points, QColor color, float thickness);

		/**
		 * @brief Get Color of Polygon2D
		 * @return Polygon2D color
		 */
		inline QColor color() const;
		inline float thickness() const;
		inline QTransform transform() const;
		inline QTransform inverseTransform() const;
		virtual QRectF boundingRect() const;
		inline bool convex();

		inline void setColor(QColor color);
		inline void setThickness(float thickness);
		inline void setTransform(QTransform& transform);
		/**
		 * @brief setAsConvex sets the polygon type as convex.
		 * @param enableInternalCheck if true, checks for convexity before setting polygon as convex
		 * and will set as non-convex in case it fails the check. If false, it sets polygon as convex
		 * without any checks (MUST BE USED WITH CAUTION)
		 */
		inline void setAsConvex(bool enableInternalCheck);

		inline void translate(float x, float y);
		void translate(const Point2D& offset);

		bool containsPoint(const Point2D &pt, Qt::FillRule rule);
		void applyRDPSmoothing(float margin);// TODO - Implement

		// Virtual functions
		virtual ItemType type() const;
		QDataStream& serialize(QDataStream& stream) const;
		QDataStream& deserialize(QDataStream& stream);

		friend QDebug operator <<(QDebug d, const Polygon2D &Polygon2D);
	protected:
		bool checkConvexity() const;
	};


	/******************************************************
	 * Polygon2D inline functions
	 *****************************************************/
	inline Polygon2D::Polygon2D()
		: _color(Qt::black), _thickness(3.0f), _isConvex(false)
	{}

	inline Polygon2D::Polygon2D(QColor color, float thickness)
		: _color(color), _thickness(thickness), _isConvex(false)
	{}

	inline Polygon2D::Polygon2D(const Polygon2D &s)
		: QPolygonF(s) , _color(s.color()), _thickness(s.thickness()),
		  _isConvex(false)
	{}

	inline Polygon2D::Polygon2D(const QVector<Point2D>& points, QColor color, float thickness)
		: QPolygonF (points), _color(color), _thickness(thickness), _isConvex(false)
	{}

	inline QColor Polygon2D::color() const
	{
		return _color;
	}

	inline float Polygon2D::thickness() const
	{
		return _thickness;
	}

	inline QTransform Polygon2D::transform() const
	{
		return _transform;
	}

	inline QTransform Polygon2D::inverseTransform() const
	{
		return _inverseTransform;
	}

	inline void Polygon2D::setColor(QColor color)
	{
		_color = color;
	}

	inline void Polygon2D::setThickness(float thickness)
	{
		mask |= isModified;
		_thickness = thickness;
	}

	inline void Polygon2D::setTransform(QTransform& transform)
	{
		mask |= isTransformed;
		_transform = transform;
		_inverseTransform = transform.inverted();
	}

	inline void Polygon2D::translate(float x, float y)
	{
		translate(QPointF(x,y));
	}
}
