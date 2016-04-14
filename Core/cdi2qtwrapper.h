#ifndef __CDI2QTWRAPPER_H__
#define __CDI2QTWRAPPER_H__

#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>
#include <QStyleOptionGraphicsItem>
#include <QWidget>
#include <QPointF>
#include <QGraphicsItem>
#include <vector>
#include "poly2tri.h"

#include <QGraphicsView>

using namespace std;

class QTouchEvent;
namespace CDI
{
	// create a list of IDs which will be assigned to qgraphicsitem which in turn can be used to
	// identify the item type
	enum GraphicsItemType {
		STROKEVIEW = 1,		
		PENSTROKEVIEW, // TODO - Deprecate
		POLYGONVIEW,
		IMAGEVIEW,
		ARROWVIEW,			// accesories
		SEARCHITEMVIEW,
		JOINTVIEW,			// component add-ons
		FORCEVIEW,
		COMPONENTVIEW,		// top-level items
		ASSEMBLYVIEW,
		UI_SELECTABLE_ACTIONS
	};

	namespace UI
	{
		enum MODE {None, Sketch, Shapes, Erase, Transform,
					 Edit, Select, Connect, PhysicsON, PhysicsOFF};

		enum EventState {Began = 1, Update, End, Cancel};
	}

	const float Z_UIVIEW			= 2.5f;
	const float Z_COMPONENTVIEW		= 1.0f;
	const float Z_TEXTVIEW			= 2.0f;	// Always keep the text on top
	const float Z_COMP_SELECT 		= 1.5f;
	const float Z_IMAGEVIEW			= 0.1f;
	const float Z_POLYGONVIEW		= 0.2f;
	const float Z_STROKEVIEW		= 0.3f;
	const float Z_JOINTVIEW			= 0.9f;

	QTouchEvent* convertToTouchEvent(QGraphicsView* view,
			 QGraphicsScene* scene, QPointF viewPos, UI::EventState eventState);

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

		PaintSettings()
		{
			pen = QPen();
			brush = QBrush();
		}

		PaintSettings(QPen penSetting, QBrush brushSetting)
		{
			pen = penSetting; brush = brushSetting;
		}

		void applyToPainter(QPainter* painter)
		{
			painter->setPen(pen);
			painter->setBrush(brush);
		}
	};


	class CommonDrawFunctions
	{
	public:

		PaintSettings arrowSettings;
		PaintSettings springSettings;
		PaintSettings jointSettings;
		PaintSettings forceSettings;
		PaintSettings touchPointSettings;
		PaintSettings polygonSettings;

		// TODO - Implement
		// \todo - Implement rendering functions
		// NOTE - make sure that the paitner is already transformed w.r.t. Item
		void drawArrow(QPointF startPos, QPointF endPos, QPainter *painter);

		void drawCurvedArrow(QPointF center, float radius, float startAngle, float endAngle, QPainter *painter);

		void  drawSpring(QPointF startPos, QPointF endPos, float height, int num_of_windings, QPainter *painter);

		// Circular disc with holes around circumference
		void drawJoint(QPointF center, float radius, QPainter *painter);

		void drawContactForces(QPointF origin, QPointF direction, float mag, QPainter *painter);

		void drawTouchPoints(QVector<QPointF> points, bool randomizeColor, QPainter *painter);

		void drawTriangulatedPolygons(QVector<QPolygonF*> polygons, QPainter *painter);

		void drawTriangulatedPolygons(vector<p2t::Triangle*> triangles, QPainter* painter);

		static CommonDrawFunctions* instance();

	private:
		CommonDrawFunctions();
		static CommonDrawFunctions* _instance;
	};
}

#endif //__CDI2QTWRAPPER_H__
