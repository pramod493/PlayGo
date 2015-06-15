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
	class Component;
	class Polygon2D : public AbstractModelItem, public QPolygonF
	{
	protected:
		QColor	_color;				/**< Polygon2D color */
		float	_thickness;			/**< Maximum thickness of Polygon2D */
		QTransform _transform;		/**< Current Polygon2D transform w.r.t. its parent */
		QTransform _inverseTransform;
		bool _isConvex;

	public:
		//-----------------------------------------------
		// Constructors/Destructors
		//-----------------------------------------------
		Polygon2D(Component *component);
		Polygon2D(Component* component, QColor color, float thickness);
		Polygon2D(const Polygon2D& p);
		Polygon2D(Component* component, const QVector<Point2D>& points, QColor color, float thickness);

		//-----------------------------------------------
		// Query/Set functions(same order in derived class)
		// Non-virtual
		//-----------------------------------------------
		/**
		 * @brief Get Color of Polygon2D
		 * @return Polygon2D color
		 */
		QColor color() const;

		/**
		 * @brief Returns the thickness of the polygon edge. Use for rendering purposes
		 * @return Edge thickness
		 */
		float thickness() const;

		/**
		 * @brief Sets the polygon color
		 * @param color - New color
		 */
		void setColor(QColor color);

		/**
		 * @brief Sets the thickess of polygon for rendering purposes
		 * @param thickness Thickness value
		 */
		void setThickness(float thickness);

		//-----------------------------------------------
		// Virtual functions (same order in derived class)
		//-----------------------------------------------

		/**
		 * @brief Returns the bounding box of the polygon in local
		 * coordinate
		 * @return AABB
		 */
		virtual QRectF boundingRect() const;

		/**
		 * @brief Che3ck for convexity of the polygon
		 * @return True if the polygon is convex. false if not convex
		 * \todo This simply uses the _isConvex value. Add checks to update the same whenever a polygon is amrked as changed
		 */
		virtual bool convex();

		/**
		 * @brief Checks if the given point is contained within the polygon or not
		 * @param pt Point position in Polygon's local co-ordinate system
		 * @param rule Polygon fill rule to determine inner region
		 * @return True, if yes. False otherwise
		 */
		bool virtual containsPoint(const Point2D &pt, Qt::FillRule rule);

		/**
		 * @brief setAsConvex sets the polygon type as convex.
		 * @param enableInternalCheck if true, checks for convexity before setting polygon as convex
		 * and will set as non-convex in case it fails the check. If false, it sets polygon as convex
		 * without any checks (MUST BE USED WITH CAUTION)
		 */
		inline virtual void setAsConvex(bool enableInternalCheck);

		ItemType type() const;

		QTransform transform() const;

		void setTransform(QTransform transform);

		QTransform inverseTransform() const;

		QDataStream& serialize(QDataStream& stream) const;
		QDataStream& deserialize(QDataStream& stream);


		//-----------------------------------------------
		// Other functions not related to query/set
		//-----------------------------------------------

		/**
		 * @brief Increments each point in the polygon by given value
		 * @param x
		 * @param y
		 */
		inline void translate(float x, float y);

		/**
		 * @brief Increments each point in the polygon by given value
		 * @param offset
		 */
		void translate(const Point2D& offset);

		/**
		 * @brief Simplify the polygon using Ramer-Douglas approx.
		 * @param margin margin value as per the smoothing algo
		 */
		void applyRDPSmoothing(float margin);// TODO - Implement

		// Virtual functions

	protected:
		bool checkConvexity() const;

		friend class Component;
		friend QDebug operator <<(QDebug d, const Polygon2D &Polygon2D);
	};


	/******************************************************
	 * Polygon2D inline functions
	 *****************************************************/
}
