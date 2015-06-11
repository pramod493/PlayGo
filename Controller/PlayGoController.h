#pragma once
#include <QObject>
#include <QList>
#include <QVector>
#include <QtAlgorithms>
#include <QTabletEvent>
#include <QColor>
#include <QTabletEvent>
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QPointF>
#include <QPolygonF>

#include <QTouchDevice>
#include <QTouchEvent>

#include "commonfunctions.h"
#include "point2dpt.h"
#include "graphicspathitem.h"
#include "graphicspolygon2d.h"

#include "sketchscene.h"
#include "sketchview.h"
#include <QDebug>

namespace CDI
{
	class CDIWindow;

	class PlayGoController : public QObject
	{
		Q_OBJECT
	public:
		enum MODE {None, Sketch, Erase, Transform, Edit, Select, Search};

	protected:
		/**********************************************************
		 * UI related variables
		 ********************************************************/
		SketchScene *_scene;
		SketchView *_view;
		CDIWindow *_toplevelWindow;

		/*******************************************************
		 * Tablet relatred operations
		 *****************************************************/
		bool _mouseModeEnabled;
		MODE _activeMode;
		QTabletEvent::TabletDevice _device;
		QTabletEvent::PointerType _pointerType;

		/*****************************************************
		 * Sketching and other drawing operations related variables
		 ****************************************************/
		QPen _defaultPen, _fillPen, _lassoPen, _highlightPen;
		QBrush _defaultBrush, _fillBrush, _lassoBrush, _highlightBrush;

		bool _isDrawingNow;
		GraphicsPathItem* _currentStroke;

		/*********************************************************
		 * Lasso related variables
		 ********************************************************/
		bool _isLassoDisplayed;
		QGraphicsPolygonItem* _lasso;
		QPolygonF _lassoPolygon;

	public:
		PlayGoController(SketchScene* scene, SketchView* view, CDIWindow *parent = NULL);

		virtual ~PlayGoController()
		{
		}

	protected:

		/**
		 * @brief initController initializes the controller variables such as pen, brush and
		 * state of sketcher
		 */
		virtual void initController();
		/// Sketching
		virtual void brushPress(QPointF pos, float pressure=1.0f, int time=0);
		virtual void brushPress(Point2DPT pos);
		virtual void brushPress(float x, float y, float pressure, int time);

		virtual void brushMove(QPointF pos, float pressure=1.0f, int time=0);
		virtual void brushMove(Point2DPT pos);
		virtual void brushMove(float x, float y, float pressure, int time);

		virtual void brushRelease(QPointF pos, float pressure=1.0f, int time=0);
		virtual void brushRelease(Point2DPT pos);
		virtual void brushRelease(float x, float y, float pressure, int time);

		virtual void eraserPress(Point2DPT pos);
		virtual void eraserMove(Point2DPT pos);
		virtual void eraserRelease(Point2DPT pos);

		virtual void lassoPress(Point2DPT pos);
		virtual void lassoMove(Point2DPT pos);
		virtual void lassoRelease(Point2DPT pos);

	public:
		virtual void sketchAction(QTabletEvent *event);
		virtual void eraseAction(QTabletEvent *event);
		virtual void selectAction(QTabletEvent *event);
		virtual void searchAction();

signals:
		void strokeComplete(GraphicsPathItem* item);
		void searchCompleted();
		void searchTriggered();
		void searchItemSelected();

	public slots:
		/**
		 * @brief onBrushSizeChange updates the pen width of default brush used for sketching
		 * @param size: new pen size (It is still subject to pressure changes)
		 */
		void setBrushWidth(int size);
		/**
		 * @brief onBrushColorChange changes the pen color of default brush used for sketching
		 * @param color: new pen color
		 */
		void setBrushColor(QColor color);

		/**
		 * @brief onSearchTrigger is called when search is trigered. This object takes pixmap
		 * as input and uses that to search
		 */
		void onSearchTrigger();
		/**
		 * @brief onSearchComplete is called when search operation is complete and manages the
		 * results display
		 */
		void onSearchComplete();

		void onTabletEventFromView(QTabletEvent *event, QGraphicsView *view);

		void onMouseEventFromScene(QGraphicsSceneMouseEvent* mouseEvent, int status);

		void setToDraw();
		void setToErase();
		void setToSelect();
		void setToEdit();

		void setMode(MODE newMode);

		void enableMouse(bool enable);
	};
}
