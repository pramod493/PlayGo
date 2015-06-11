#pragma once

#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QPointF>
#include <QGraphicsItem>

namespace CDI
{
	// create a list of IDs which will be assigned to qgraphicsitem which in turn can be used to
	// identify the item type
	enum GraphicsItemType {
		STROKEVIEW = 1,
		POLYGONVIEW,
		IMAGEVIEW,
		COMPONENTVIEW,
		ASSEMBLYVIEW
	};

	// This section will create various paint
	// functions which will be helpful in rendering
	// joint and other items on screen. Its painting
	// will be completely dictated by parent items
	
	/**
	 * @brief The PaintSettings struct contain a collection of different style pen/brushes
	 */
	struct PaintSettings {
		QPen pen;
		QBrush brush;
	};
	

	class CommonDrawFunction
	{
		PaintSettings arrowSettings;
		PaintSettings springSettings;
		PaintSettings jointSettings;
		PaintSettings forceSettings;
		PaintSettings touchPointSettings;
		PaintSettings polygonSettings;

		// TODO - Implement
		// \todo - Implement rendering functions
		// NOTE - make sure that the paitner is already transformed w.r.t. Item
		void drawArrow(QPointF startPos, QPointF endPos, QPainter *painter)
		{}

		void drawCurvedArrow(QPointF center, float radius, float startAngle, float endAngle, QPainter *painter)
		{}

		void  drawSpring(QPointF startPos, QPointF endPos, float width, int num_of_windings, QPainter *painter)
		{}

		// Circular disc with holes around circumference
		void drawJoint(QPointF center, QPointF radius, QPainter *painter)
		{}

		void drawContactForces(QPointF origin, QPointF direction, float mag, QPainter *painter)
		{}

		void drawTouchPoints(QVector<QPointF> points, bool randomizeColor, QPainter *painter)
		{

		}

		void drawTriangulatedPolygons(QVector<QPolygonF*> polygons, QPainter *painter)
		{

		}
	};
}
