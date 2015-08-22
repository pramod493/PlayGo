#include "cdi2qtwrapper.h"
#include "commonfunctions.h"
#include <QTouchEvent>
#include <QTouchDevice>
#include <QMap>

namespace CDI
{
	int sessionId = 0;
	bool touchConvertInit = false;
	QMap<int, QTouchEvent::TouchPoint> *cdTouchPointMap;

	QTouchEvent* convertToTouchEvent(QGraphicsView* view,
			 QGraphicsScene* scene, QPointF viewPos, UI::EventState eventState)
	{
		if (!touchConvertInit)
		{
			cdTouchPointMap = new QMap<int, QTouchEvent::TouchPoint>();
			touchConvertInit = true;
		}

		// Use QTuio approach to convert cursor info to touch
		if (sessionId == 0 || eventState == UI::Began) sessionId = 1;	// Always use this ID

		QEvent::Type eventType = QEvent::None;

		QTouchEvent::TouchPoint touchPoint(sessionId);

		touchPoint.setPressure(1.0);
		touchPoint.setPos(viewPos);
		touchPoint.setScenePos(view->mapToScene(viewPos.toPoint()));
		touchPoint.setSceneRect(scene->sceneRect());

		Qt::TouchPointStates touchPointStates;

		switch (eventState)
		{
			case UI::Began :
			{
				touchPoint.setState(Qt::TouchPointPressed);
				touchPointStates = Qt::TouchPointPressed;
				eventType = QEvent::TouchBegin;

				// Since it is start, set the last frame value to be same as current one
				touchPoint.setLastPos(viewPos);
				touchPoint.setLastScenePos(touchPoint.scenePos());

				touchPoint.setStartPos(viewPos);
				touchPoint.setStartScenePos(touchPoint.scenePos());

				cdTouchPointMap->insert(sessionId, touchPoint);
				break;
			}
			case UI::Update :
			{
				touchPointStates = Qt::TouchPointMoved;	// Need to add stationary options too
				touchPoint.setState(Qt::TouchPointMoved);

				touchPoint.setStartPos(cdTouchPointMap->value(sessionId).startPos());
				touchPoint.setStartScenePos(cdTouchPointMap->value(sessionId).startScenePos());

				touchPoint.setLastPos(cdTouchPointMap->value(sessionId).pos());
				touchPoint.setLastScenePos(cdTouchPointMap->value(sessionId).scenePos());

				cdTouchPointMap->insert(sessionId, touchPoint);	// clear last one
				break;
			}
			case UI::End :
			{
				touchPointStates = Qt::TouchPointReleased;	// Need to add stationary options too
				touchPoint.setState(Qt::TouchPointReleased);

				touchPoint.setStartPos(cdTouchPointMap->value(sessionId).startPos());
				touchPoint.setStartScenePos(cdTouchPointMap->value(sessionId).startScenePos());

				touchPoint.setLastPos(cdTouchPointMap->value(sessionId).pos());
				touchPoint.setLastScenePos(cdTouchPointMap->value(sessionId).scenePos());

				cdTouchPointMap->insert(sessionId, touchPoint);	// clear last one

				sessionId = 0;
				break;
			}
			case UI::Cancel :
			{
				break;
			}
		}

		QTouchDevice touchDevice = QTouchDevice();
		touchDevice.setType(QTouchDevice::TouchScreen);

		QList<QTouchEvent::TouchPoint> points; points.push_back(touchPoint);	// It's only one
		//QTouchEvent* touchEvent = new QTouchEvent(1);
		return NULL;
	}

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
		Q_UNUSED(triangles)
		Q_UNUSED(painter)
	}

	CommonDrawFunctions* CommonDrawFunctions::_instance = NULL;

	CommonDrawFunctions* CommonDrawFunctions::instance()
	{
		if (!_instance)
			_instance = new CommonDrawFunctions;
		return _instance;
	}

	CommonDrawFunctions::CommonDrawFunctions()
	{
		arrowSettings = PaintSettings();
		springSettings = PaintSettings();
		jointSettings = PaintSettings();
		forceSettings = PaintSettings();
		touchPointSettings = PaintSettings();
		polygonSettings = PaintSettings();
	}
}
