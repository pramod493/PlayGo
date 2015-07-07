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
#include <QRectF>
#include <QTouchDevice>
#include <QTouchEvent>

#include "commonfunctions.h"
#include "point2dpt.h"
#include "stroke.h"
#include "polygon2d.h"

#include "searchmanager.h"

#include "sketchscene.h"
#include "sketchview.h"
#include "searchview.h"
#include "modelviewtreewidget.h"
#include <QDebug>

namespace CDI
{
	class CDIWindow;
    namespace UI
    {
		enum MODE {None, Sketch, Shapes, Erase, Transform,
                   Edit, Select, Connect};
    }

    /**
     * @brief Main controller object. This receives input events from View and Scene
     * and modifies the view as well as scene based on that.
     * @remarks We can also implement state machine to manage various modes
     * but currently using UI::MODE flag to determine the operations
     */
	class PlayGoController : public QObject
	{
		Q_OBJECT

	protected:
		/**********************************************************
		 * UI related variables
		 ********************************************************/
		SketchScene*	_scene;
		SketchView*		_view;
		Page*			_page;
		CDIWindow*		_toplevelWindow;

		/*******************************************************
		 * Tablet relatred operations
		 *****************************************************/
		bool _mouseModeEnabled;
        UI::MODE _activeMode;
		QTabletEvent::TabletDevice _device;
		QTabletEvent::PointerType _pointerType;

		/*****************************************************
		 * Sketching and other drawing operations related variables
		 ****************************************************/
		QPen _defaultPen, _fillPen, _lassoPen, _highlightPen;
		QBrush _defaultBrush, _fillBrush, _lassoBrush, _highlightBrush;

		bool _isDrawingNow;
		Stroke* _currentStroke;

		/*********************************************************
		 * Lasso related variables
		 ********************************************************/
		bool _isLassoDisplayed;
		QGraphicsPolygonItem* _lasso;
		QPolygonF _lassoPolygon;
		bool _itemHighlighted;

		ModelViewTreeWidget* tree;

        /*********************************************************
         * Search related variables
         ********************************************************/
		SearchView* searchView;

	public:
		PlayGoController(SketchScene* scene, SketchView* view, CDIWindow *parent = NULL);

		virtual ~PlayGoController()
		{
			if (tree != NULL) delete tree;
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

        /**
         * @brief Handles updates whenever MODE is changed.
         * Note that this function does not actually change the mode
         * @param oldmode Current mode of the controller
         * @param newmode New mode of the controller
         */
        virtual void onModeChange(UI::MODE oldmode, UI::MODE newmode);

	public:
		virtual void sketchAction(QTabletEvent *event);
		virtual void eraseAction(QTabletEvent *event);
		virtual void selectAction(QTabletEvent *event);
		virtual void searchAction();

	signals:
		void signalStrokeComplete(Stroke* item);
		void signalSearchComplete();
		void signalSearchBegin();
		void signalSearchItemSelect();

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
		void onSearchBegin();
		/**
		 * @brief onSearchComplete is called when search operation is complete and manages the
		 * results display
		 */
		void onSearchComplete();

		void onSearchItemSelect(SearchResult* result);

        /**
         * @brief Receives tablet/pen event from view
         * @param event QTabletEvent
         * @param view QGraphicsView from which the event originiated
         */
		void onTabletEventFromView(QTabletEvent *event, QGraphicsView *view);

        /**
         * @brief Receives mouse event from scene
         * @param mouseEvent QGraphicsSceneMouseEvent
         * @param status QGraphicsScene from which the event originiated
         */
		void onMouseEventFromScene(QGraphicsSceneMouseEvent* mouseEvent, int status);

        /**
         * @brief Receives touch event from view
         * @param event QTouchEvent
         * @param view QGraphicsView from where event originiated
         */
		void onTouchEventFromView(QTouchEvent* event, QGraphicsView *view);

		void setToDraw();
		void setToErase();
		void setToSelect();
		void setToEdit();

        void setMode(UI::MODE newMode);

		void enableMouse(bool enable);

		void clearCurrentScene();

		//////////////////////////////////////////////
		// Experimental
		void drawMenusOnView(QPainter * painter, const QRectF & rect);

		// Dropbox sync for file change
		void onExternalImageAdd(const QString& path);

		void loadImage(QString imagePath);

		void startSimulation();

		void pauseSimulation();

		void loadCamera();

	};
}
