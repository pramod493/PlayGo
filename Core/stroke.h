#pragma once
#include <commonfunctions.h>
#include <abstractmodelitem.h>
#include <QDataStream>
#include <point2dpt.h>
#include <QVector>
#include <QColor>

namespace CDI
{
	/**
	 * @brief The Stroke class
	 * Stroke the points, color and thickness information for stroke
	 */
	class Stroke : public QVector<Point2DPT>, public AbstractModelItem
	{
	protected:
		QColor _color;				/**< Stroke color */
		float _thickness;			/**< Maximum thickness of stroke */
		QTransform _transform;		/**< Current stroke transform w.r.t. its parent */

	public:
		inline Stroke();
		inline Stroke(QColor color, float thickness);
		inline Stroke(const Stroke& s);
		inline Stroke(const QVector<Point2DPT>& points, QColor color, float thickness);

		/**
		 * @brief Get Color of stroke
		 * @return Stroke color
		 */
		inline QColor color() const;
		inline float thickness() const;
		inline QTransform transform() const;

		inline void setColor(QColor color);
		inline void setThickness(float thickness);
		inline void setTransform(QTransform& transform);

		inline void translate(float x, float y);
		void translate(const Point2D& offset);

		bool containsPoint(const Point2D &pt, SelectionType rule, float margin);

		// Virtual functions
		virtual ItemType type() const;
		QDataStream& serialize(QDataStream& stream) const;
		QDataStream& deserialize(QDataStream& stream);

		friend QDebug operator <<(QDebug d, const Stroke &stroke);
	};

	/******************************************************
	 * Stroke inline functions
	 *****************************************************/
	inline Stroke::Stroke()
		: _color(Qt::black), _thickness(3.0f)
	{}

	inline Stroke::Stroke(QColor color, float thickness)
		: _color(color), _thickness(thickness)
	{}

	inline Stroke::Stroke(const Stroke &s)
		: QVector<Point2DPT>(s) , _color(s.color()), _thickness(s.thickness())
	{}

	inline Stroke::Stroke(const QVector<Point2DPT>& points, QColor color, float thickness)
		: QVector<Point2DPT> (points), _color(color), _thickness(thickness)
	{}

	inline QColor Stroke::color() const
	{
		return _color;
	}

	inline float Stroke::thickness() const
	{
		return _thickness;
	}

	inline QTransform Stroke::transform() const
	{
		return _transform;
	}

	inline void Stroke::setColor(QColor color)
	{
		_color = color;
	}

	inline void Stroke::setThickness(float thickness)
	{
		mask |= isModified;
		_thickness = thickness;
	}

	inline void Stroke::setTransform(QTransform& transform)
	{
		mask |= isTransformed;
		_transform = transform;
	}

	inline void Stroke::translate(float x, float y)
	{
		translate(QPointF(x,y));
	}
}
