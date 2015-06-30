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
	class Component;
	/**
	 * @brief The Stroke class
	 * Stroke the points, color and thickness information for stroke
	 */
	class Stroke : public AbstractModelItem, public QVector<Point2DPT>
	{
	protected:
		QColor	_color;				/**< Stroke color */
		float	_thickness;			/**< Maximum thickness of stroke */
		QTransform _transform;		/**< Current stroke transform w.r.t. its parent */
		QTransform _inverseTransform;

		float _x_min, _x_max, _y_min, _y_max;
		QRectF aabb;

	public:
		//-----------------------------------------------
		// Constructors/Destructors
		//-----------------------------------------------
		/**
		 * @brief Stroke Creates an empty Stroke object with color black and thickness 3.0
		 */
		Stroke(Component* component);

		/**
		 * @brief Stroke Creates an empty strokes with given color and thickness
		 * @param color Stroke color
		 * @param thickness Stroke thickness
		 */
		Stroke(Component* component, QColor color, float thickness);

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
		Stroke(Component* component, const QVector<Point2DPT>& points, QColor color, float thickness);


		virtual ~Stroke();

		//-----------------------------------------------
		// Query/Set functions(same order in derived class)
		// Non-virtual
		//-----------------------------------------------
		/**
		 * @brief push_points Use this method to add points to stroke
		 * @param pt
		 */
        void push_point(Point2DPT pt);

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
		 * @brief containsPoint checks if the given point lies with margin of the stroke path
		 * @param pt Point coordinates in parent's local coordinate system
		 * @param rule - Not used
		 * @param margin Maximum allowed radial offset from the point
		 * @return True if distance between point and Stroke is less than margin, Else false
         * @remarks Ensure that point pt is mapped to the local coordinate system.
         * Use globalTransform().inverted() to map points to local
		 */
		bool containsPoint(const Point2D &pt, SelectionType rule, float margin);

		/**
		 * @brief Checks if the stroke is fully contained withing a given polygon
		 * @param polygon Envelope polygon mapped to stroke's local coordinate system
		 * @param percentmatch Minimum overlap required to mark as true
		 * @return True, when contianed. False otherwise
		 */
		bool isContainedWithin(QPolygonF* polygon, float percentmatch = 1.0f);

		//-----------------------------------------------
		// Virtual functions (same order in derived class)
		//-----------------------------------------------
		/**
		 * @brief Returns the bounding rectangle of the stroke in terms
		 * of local transform
		 * @return AABB
		 */
		virtual QRectF boundingRect();

		/**
		 * @brief type Returns Item type
		 * @return STROKE
		 */
		ItemType type() const;

		QTransform transform() const;

		/**
		 * @brief setTransform Set Stroke local transform
		 * @param transform New transform
		 */
		void setTransform(QTransform t);

		/**
		 * @brief inverseTransform get inverse of Stroke's transformation matrix
		 * @return Inverse transform
		 */
		QTransform inverseTransform() const;

		QDataStream& serialize(QDataStream& stream) const;

		QDataStream& deserialize(QDataStream& stream);

		//-----------------------------------------------
		// Other functions not related to query/set
		//-----------------------------------------------
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

	protected:
		/**
		 * @brief Updates/recalculates the internal quantities when isModified bit is ON
		 */
		void updateWhenModified();

		/**
		 * @brief Updates the AABB. It calculate the bounding box of points and expands by thickness
		 * on all sides
		 */
		void recalculateAABB();

		friend class Component;

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

	inline void Stroke::setColor(QColor color)
	{
		_color = color;
	}

	inline void Stroke::setThickness(float thickness)
	{
		mask |= isModified;
		_thickness = thickness;
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

//Q_DECLARE_METATYPE(CDI::Stroke)
