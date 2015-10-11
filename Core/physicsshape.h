#pragma once
#include "box2dworld.h"
#include "abstractmodelitem.h"
#include "commonfunctions.h"
#include "poly2tri.h"
#include <vector>
#include <QPainter>
#include <QPolygonF>

namespace CDI
{
	class Polygon2D;
	class Stroke;

	/**
	 * @brief The Triangle class stores 3 vertices required for a traingle
	 * @remarks This is better suited to be clubbed with commonfunctions.h
	 */
	class Triangle {
	public:
		Point2D points[3];

		Triangle()
		{
			points[0] = Point2D();
			points[1] = Point2D();
			points[2] = Point2D();
		}

		Triangle(Point2D& a, Point2D& b, Point2D& c)
		{
			points[0] = a;
			points[1] = b;
			points[2] = c;
		}

		Triangle(const Triangle& copy)
		{
			points[0] = copy.points[0];
			points[1] = copy.points[1];
			points[2] = copy.points[2];
		}

		Triangle(p2t::Triangle* p2tTria)
		{
			p2t::Point *pt = 0;
			for (int i=0; i< 3; i++)
			{
				pt = p2tTria->GetPoint(i);
				points[i] = Point2D(pt->x, pt->y);
			}
		}

		void applyTransform(QTransform &t)
		{
			points[0] = t.map(points[0]);
			points[1] = t.map(points[1]);
			points[1] = t.map(points[2]);
		}

		QDataStream& serialize(QDataStream& stream) const
		{
			for (int i=0; i< 3; i++)
			{
				stream << points[i];
			}
			return stream;
		}

		QDataStream& deserialize(QDataStream& stream)
		{
			for (int i=0; i< 3; i++)
			{
				stream >> points[i];
			}
			return stream;
		}

		void paint(QPainter* painter)
		{
			painter->drawLine(points[0],points[1]);
			painter->drawLine(points[1],points[2]);
			painter->drawLine(points[2],points[0]);
		}
	};

	/**
	 * @brief The PhysicsShape class stores the shape data required
	 * for collision detection as well as for dynamics calculation in
	 * Box2D physics engine.
	 * This object can be individually saved be saved/loaded from disk
	 * so as to reduce the time spent in computations.
	 */
	class PhysicsShape : public Item
	{
	public:
		bool scaledToPhysics;

		vector<Triangle*> trias;

		vector<QPolygonF*> polygons;

	protected:
		QUuid _id;

		float _currentscale;

	public:
		/**
		 * @brief Create object to store shape of physics objects
		 */
		PhysicsShape();

		PhysicsShape(vector<p2t::Triangle*> p2tTrias);

		PhysicsShape(const PhysicsShape& copy);

		virtual ~PhysicsShape();

		ItemType type() const;

		QUuid id() const;

		QDataStream& serialize(QDataStream &stream) const;

		QDataStream& deserialize(QDataStream &stream);

		bool addShapeToBody(b2Body* body, float density);

		float scale();

		void setScale(float val);

	protected:

	public:
		/**
		 * @brief Create Triangulated physics shape from QPixmap
		 * @param pixmap
		 * @return
		 */
		static PhysicsShape* createShapeFromPixmap(QPixmap& pixmap);

		// TODO - Not using this functionality at present. Later
		// static PhysicsShape* createShapeFromPolygon(Polygon2D* polygon);

		static PhysicsShape* createShapeFromTriangles(std::vector<p2t::Triangle*> triangles);
	};
}
