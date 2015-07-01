#include "cdi2qtwrapper.h"
#include "commonfunctions.h"

namespace CDI
{
	// TODO - Implement
	// \todo - Implement rendering functions
	// NOTE - make sure that the paitner is already transformed w.r.t. Item
	void CommonDrawFunctions::drawArrow(QPointF startPos, QPointF endPos, QPainter *painter)
	{
		painter->drawLine(startPos, endPos);
	}

	void CommonDrawFunctions::drawCurvedArrow(QPointF center, float radius, float startAngle, float endAngle, QPainter *painter)
	{
		painter->drawArc(static_cast<int>(center.x()-radius),
						 static_cast<int>(center.y()-radius),
						 static_cast<int>(2*radius),
						 static_cast<int>(2*radius),
						 static_cast<int>(startAngle*16),
						 static_cast<int>((endAngle-startAngle)*16));
	}

	void CommonDrawFunctions::drawSpring(QPointF startPos, QPointF endPos, float height, int num_of_windings, QPainter *painter)
	{
		// To get normal
		// change sign of y-component
		// Exchange the x and y co-ords
		QPointF dispVec = endPos - startPos;
		QPointF normalVector = QPointF(-dispVec.y(), dispVec.x()) * (height/magnitude(&dispVec));
		dispVec = dispVec / num_of_windings;
		// TODO - Lots of stuff here
		painter->drawLine(startPos, endPos);
	}

	// Circular disc with holes around circumference
	void CommonDrawFunctions::drawJoint(QPointF center, float radius, QPainter *painter)
	{
		QRectF rect = QRectF(center.x()-radius, center.y()-radius,
							 2*radius, 2*radius);
		painter->drawEllipse(rect);
	}

	void CommonDrawFunctions::drawContactForces(QPointF origin, QPointF direction, float mag, QPainter *painter)
	{
		painter->drawLine(origin, origin+ mag*direction);
	}

	void CommonDrawFunctions::drawTouchPoints(QVector<QPointF> points, bool randomizeColor, QPainter *painter)
	{
		float radius = 3.0f;
		QVector<QPointF>::const_iterator iter;
		for (iter = points.constBegin();
			 iter != points.constEnd();
			 ++iter)
		{
			QPointF center = (*iter);
			QRectF rect = QRectF(center.x()-radius, center.y()-radius,
								 2*radius, 2*radius);
			painter->drawEllipse(rect);
		}
	}

	void CommonDrawFunctions::drawTriangulatedPolygons(QVector<QPolygonF*> polygons, QPainter *painter)
	{
		QVector<QPolygonF*>::const_iterator iter;
		for (iter = polygons.constBegin();
			 iter != polygons.constEnd();
			 ++iter)
		{
			QPolygonF* polygon = (*iter);
			painter->drawPolygon(*polygon);
		}
	}

	void CommonDrawFunctions::drawTriangulatedPolygons(vector<p2t::Triangle*> triangles, QPainter* painter)
	{

	}

}
