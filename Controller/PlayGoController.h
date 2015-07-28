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
#include <QLineEdit>
#include <QValidator>
#include <QIntValidator>
#include <QCheckBox>

#include "commonfunctions.h"
#include "point2dpt.h"
#include "stroke.h"
#include "polygon2d.h"

#include "searchmanager.h"

#include "sketchscene.h"
#include "sketchview.h"
#include "searchview.h"
#include "modelviewtreewidget.h"

#include <QToolBar>
#include "penstroke.h"
#include <QGraphicsLineItem>
#include "forceitem.h"
#include <QMessageBox>
#include <box2dworld.h>
#include "pdollarrecognizer.h"
#include "polygon2d.h"

#include <QDebug>
#include <QTabletEvent>
namespace CDI
{
	class CDIWindow;
	class ConnectController;
	class TouchAndHoldController;

	namespace UI
	{
		enum MODE {None, Sketch, Shapes, Erase, Transform,
				   Edit, Select, Connect};

		enum EventState {Began = 1, Update, End, Cancel};
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
		enum ConnectionMode {GestureSketch, StaticJoint, HingeJoint, SliderJoint, FixedJoint/*Sets body as static*/,
							 SpringJoint, ApplyForce};	// Leave it to be private

	protected:
		/**********************************************************
		 * UI related variables
		 ********************************************************/
		SketchScene*	_scene;
		SketchView*		_view;
		QWidget*		_viewport;
		Page*			_page;
		CDIWindow*		_toplevelWindow;

		/*******************************************************
		 * Tablet relatred operations
		 *****************************************************/
		bool _mouseModeEnabled;
		UI::MODE _activeMode;
		QTabletEvent::TabletDevice _device;

		/*****************************************************
		 * Sketching and other drawing operations related variables
		 ****************************************************/
		QPen _defaultPen, _fillPen, _lassoPen, _highlightPen;
		QBrush _defaultBrush, _fillBrush, _lassoBrush, _highlightBrush;

		bool _isDrawingStroke;
		bool _isDrawingPolygon;
		Stroke* _currentStroke;
		Polygon2D* _currentPolygon;

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

		/*********************************************************
		 * Search related variables
		 ********************************************************/
		QToolBar* connectionOptionsToolbar;

		ConnectionMode _activeConnectionMode;

		QList<PenStroke*> _tmpStrokes;

		PenStroke* _currentConnectStroke;

		PDollarRecognizer *sketchRecognizer;

		QCheckBox* enableMotorCheckbox;
		QLineEdit* motorSpeed;
		QLineEdit* motorTorque;

		/*********************************************************
		 * Slider joint settings
		 ********************************************************/
		Component* _sliderComponentA;
		Component* _sliderComponentB;
		QGraphicsLineItem* _sliderLineItem;
		QPointF _sliderStartPos;
		QPointF _sliderEndPos;

		// Limit the slider to startPos and endPos

		/*********************************************************
		 * Edit component and joint settings
		 ********************************************************/
		bool _tapOverrideEnabled;
		TouchAndHoldController* touchholdController;

	private:
		ForceGraphicsItem* forceLine;

	public:
		PlayGoController(SketchView* view, CDIWindow *parent = NULL);

		virtual ~PlayGoController();

		SketchScene* scene() const { return _scene; }
		SketchView* view() const { return _view; }
		Page* page() const { return _page; }

	protected:

		/**
		 * @brief initController initializes the controller variables such as pen, brush and
		 * state of sketcher
		 */
		virtual void initController();
		/// Sketching
		virtual void brushPress(QPointF pos, float pressure=1.0f, int time=0);
		virtual void brushMove(QPointF pos, float pressure=1.0f, int time=0);
		virtual void brushRelease(QPointF pos, float pressure=1.0f, int time=0);

		/// Polygon drawing
		virtual void shapePress(QPointF pos);
		virtual void shapeMove(QPointF pos);
		virtual void shapeRelease(QPointF pos);

		/// Erase items
		virtual void eraserPress(QPointF pos);
		virtual void eraserMove(QPointF pos);
		virtual void eraserRelease(QPointF pos);

		/// Select objects
		virtual void lassoPress(QPointF pos);
		virtual void lassoMove(QPointF pos);
		virtual void lassoRelease(QPointF pos);

		virtual void connectPress(QPointF scenePos);
		virtual void connectMove(QPointF scenePos);
		virtual void connectRelease(QPointF scenePos);

		void createConnectionsToolbar();
		void showConnectionsToolbar();
		void hideConnectionsToolbar();

		/**
		 * @brief Handles updates whenever MODE is changed.
		 * Note that this function does not actually change the mode
		 * @param oldmode Current mode of the controller
		 * @param newmode New mode of the controller
		 */
		virtual bool onModeChange(UI::MODE oldmode, UI::MODE newmode);

		bool eventFilter(QObject* obj, QEvent* event);

		QList<Component*> getSelectableComponentsByPhysics(QPointF scenePos);

	public:
		virtual void sketchAction(QTabletEvent *event);
		virtual void shapeAction(QTabletEvent *event);
		virtual void eraseAction(QTabletEvent *event);
		virtual void selectAction(QTabletEvent *event);
		virtual void connectAction(QTabletEvent *event);
		virtual void searchAction();

		bool isTapOverrideEnabled() const;
		void setTapOverride(bool value);
		void overrideOnTapAndHold(QTapAndHoldGesture* gesture);

	signals:
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

		void onMouseEventFromScene(QGraphicsSceneMouseEvent* mouseEvent, int status);*/

		/**
		 * @brief Receives touch event from view
		 * @param event QTouchEvent
		 * @param view QGraphicsView from where event originiated
		 */
		bool onTouchEventFromView(QTouchEvent* event);

		/**
		 * @brief Manages gesture events received from viewport.
		 * @param event QGestureEvent
		 * @return
		 * @remarks Should the events be accepted by Component object in the Scene
		 */
		void onGestureEventFromView(QGestureEvent* event);

		void connectionModeReset();

		void setToDraw();
		void setToShape();
		void setToErase();
		void setToConnectorMode();
		void setToSelect();
		void setToEdit();

		void setModeScribble();
		void setModeLockItem();
		void setModeHingeJoint();
		void setModeSliderJoint();
		void setModeSpringJoint();
		void setModeForce();

		void setMode(UI::MODE newMode);

		void enableMouse(bool enable);

		void clearCurrentScene();

		//////////////////////////////////////////////
		// Experimental
		void drawMenusOnView(QPainter * painter, const QRectF & rect);

		// Dropbox sync for file change
		void onExternalImageAdd(const QString& path);

		void loadImage(QString imagePath);

		void loadImage(QString imagePath, QObject* obj, QDropEvent *event);

		void startSimulation();

		void pauseSimulation();

		void loadCamera();

		friend class ConnectController;
		friend class TouchAndHoldController;
	};
}
