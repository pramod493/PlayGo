#pragma once
#include <commonfunctions.h>
#include <abstractmodelitem.h>
#include <QDataStream>
#include <point2dpt.h>
#include <QVector>
#include <QColor>
#include <QRectF>

namespace CDI
{
	/**
	 * @brief The Stroke class
	 * Stroke the points, color and thickness information for stroke
	 */
	class Stroke : public AbstractModelItem, public QVector<Point2DPT>
	{
		Q_OBJECT
		Q_PROPERTY(QColor color READ color WRITE setColor)
		Q_PROPERTY(float thickness READ thickness WRITE setThickness)
		Q_PROPERTY(QTransform transform READ transform WRITE setTransform)
		Q_PROPERTY(QTransform inverseTransform READ inverseTransform)
	protected:
		QColor _color;				/**< Stroke color */
		float _thickness;			/**< Maximum thickness of stroke */
		QTransform _transform;		/**< Current stroke transform w.r.t. its parent */
		QTransform _inverseTransform;

		// TODO - Implement push_point function so that we can track changes to bounding rect
		float _x_min, _x_max, _y_min, _y_max;
		QRectF aabb;
	public:

		/**
		 * @brief Stroke Creates an empty Stroke object with color black and thickness 3.0
		 */
		Stroke();

		/**
		 * @brief Stroke Creates an empty strokes with given color and thickness
		 * @param color Stroke color
		 * @param thickness Stroke thickness
		 */
		Stroke(QColor color, float thickness);

		/**
		 * @brief Stroke Copy constructor of Stroke
		 * @param s Stroke to copy
		 */
		Stroke(const Stroke& s);

		/**
		 * @brief Stroke Constructor for creating strokes from vector of points
		 * @param points List of points
		 * @param color Stroke color
		 * @param thickness Stroke thickness
		 * @remarks list of points is copied, and therefore passed value can be safely discarded
		 */
		Stroke(const QVector<Point2DPT>& points, QColor color, float thickness);

		/**
		 * @brief push_points Use this method to add points to stroke
		 * @param pt
		 */
		void push_point(Point2DPT& pt);
		/**
		 * @brief Get Color of stroke
		 * @return Stroke color
		 */
		inline QColor color() const;

		/**
		 * @brief thickness Get Stroke thickness
		 * @return Stroke thickness
		 */
		inline float thickness() const;

		/**
		 * @brief transform Get Stroke transform
		 * @return Stroke transform
		 */
		inline QTransform transform() const;

		/**
		 * @brief inverseTransform get inverse of Stroke's transformation matrix
		 * @return Inverse transform
		 */
		inline QTransform inverseTransform() const;

		QRectF boundingRect();

		/**
		 * @brief setColor Set Stroke color
		 * @param color new stroke color
		 */
		inline void setColor(QColor color);

		/**
		 * @brief setThickness Set stroke thickness
		 * @param thickness new Stroke thickness
		 */
		inline void setThickness(float thickness);

		/**
		 * @brief setTransform Set Stroke local transform
		 * @param transform New transform
		 */
		inline void setTransform(QTransform& transform);

		/**
		 * @brief translate translates each point in stroke by given value
		 * @param x X-axis offset in local coords
		 * @param y Y-axis offset in local coords
		 * @remarks This updates the co-ordinate value of each stroke point
		 */
		inline void translate(float x, float y);

		/**
		 * @brief translate translates each point in Stroke by given value
		 * @param offset offset in local coords
		 */
		void translate(const Point2D& offset);

		/**
		 * @brief containsPoint checks if the given point lies with margin of the stroke path
		 * @param pt Point coordinates in parent's local coordinate system
		 * @param rule - Not used
		 * @param margin Maximum allowed radial offset from the point
		 * @return True if distance between point and Stroke is less than margin, Else false
		 * @remarks It transforms the point pt internally to local coordinates
		 */
		bool containsPoint(const Point2D &pt, SelectionType rule, float margin);

		/**
		 * @brief applySmoothing Apply laplacian smoothing to the stroke
		 * @param order Smoothing order
		 */
		void applySmoothing(int order);// TODO - Implement
		/**
		 * @brief mergeWith always merges the given stroke's start point with end point
		 * of the given stroke
		 * @param stroke Stroke which will be merged with this one
		 * @todo Merging of strokes is not implemented. Will return false
		 */
		bool mergeWith(Stroke* stroke, bool joinAtEnd = false);

		/**
		 * @brief type Returns Item type
		 * @return ItemType::STROKE
		 */
		ItemType type() const;

		QDataStream& serialize(QDataStream& stream) const;
		QDataStream& deserialize(QDataStream& stream);

	protected:
		/**
		 * @brief updateWhenModified updates/recalculates the internal quantities when isModified bit is ON
		 */
		void updateWhenModified();

		void recalculateAABB();

		friend QDebug operator<<(QDebug d, const Stroke &stroke);

		// Allows merging of two strokes into a new one
		friend Stroke* merge(Stroke *s1, Stroke *s2, bool addS1Forward, bool addS2Forward);
	};

	/******************************************************
	 * Stroke inline functions
	 *****************************************************/
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

	inline QTransform Stroke::inverseTransform() const
	{
		return _inverseTransform;
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
		_inverseTransform = transform.inverted();
		emit transformChanged(this);
	}

	inline void Stroke::translate(float x, float y)
	{
		translate(QPointF(x,y));
	}

	/**
	 * @brief merge merges two Stroke objects into one new Stroke object.
	 * Start point of s2 is appended to end point of s1
	 * Color nad thickness of new stroke is same as that of first stroke
	 * @param s1 First stroke
	 * @param s2 Second stroke
	 * @param addS1Forward add the first  stroke in forward order
	 * @param addS2Forward add the second stroke in forward order
	 * @return new merged stroke
	 * \todo Add options to merge s1 and s2 in any direction. Simply reverse the order o array iteration
	 */
	Stroke* merge(Stroke *s1, Stroke *s2, bool addS1Forward = true, bool addS2Forward = true);
}

Q_DECLARE_METATYPE(CDI::Stroke)
