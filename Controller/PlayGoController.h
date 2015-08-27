#ifndef __PLAYGOCONTROLLER_H__
#define __PLAYGOCONTROLLER_H__

#include <QObject>

#include <QList>
#include <QVector>
#include <QtAlgorithms>

#include <QColor>
#include <QPen>
#include <QBrush>
#include <QPainter>

#include <QTouchDevice>
#include <QTouchEvent>
#include <QTabletEvent>

#include <QToolBar>
#include <QLineEdit>
#include <QValidator>
#include <QIntValidator>
#include <QCheckBox>
#include <QMessageBox>

#include "playgocore.h"
#include <QGraphicsLineItem>

#include "sketchscene.h"
#include "sketchview.h"
#include "modelviewtreewidget.h"

#include "box2dworld.h"

#include <QStateMachine>
#include <QState>

namespace CDI
{
	class CDIWindow;
	class ConnectController;
	class TouchAndHoldController;
	class cdState;
	class cdSearchGraphicsItem;
	class LoadModel;
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
		//bool _mouseModeEnabled;
		// Need not use mouse mode because we can reject touch->mouse events
		// use _isStylusNearby to differentiate between mouse event from stylus
		// and from stylus
		UI::MODE _activeMode;
		UI::MODE _lastActiveMode;
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

		// Widgets
		ModelViewTreeWidget* tree;

		/*********************************************************
		 * Search related variables
		 ********************************************************/
		QGraphicsItemGroup* searchItemGroup;
		QAction *hideSearchResultAction;
		bool _searchResultsDisplayed;

		/*********************************************************
		 * Physics manager settings
		 ********************************************************/
		unsigned int _physicsMask;	/**< Decides which physics options will be active*/

		/*********************************************************
		 * joint related variables
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
		Component* _currentTouchEnabledComponent;
		bool _isStylusNearby;

		unsigned int p_physicsmask;

	public:
		/**
		 * @brief Instantiate controller for specific Page and SketchView
		 * @param view SketchView reference
		 * @param parent Main window (contains all the UI parameters
		 */
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

		virtual void gestureSketchModeFilter(QPointF scenePos, UI::EventState eventState);
		virtual void staticJointModeFilter(QPointF scenePos, UI::EventState eventState);
		virtual void hingeJointModeFilter(QPointF scenePos, UI::EventState eventState);
		virtual void sliderJointModeFilter(QPointF scenePos, UI::EventState eventState);
		virtual void forceModeFilter(QPointF scenePos, UI::EventState eventState);

		void createConnectionsToolbar();
		void showConnectionsToolbar();
		void hideConnectionsToolbar();

		/**
		 * @brief getMotorParams returns the current joint motor settings
		 * @param motorEnable True, if motor enabled
		 * @param speed Speed set in the toolbar
		 * @param torque Maximum torque allowed
		 */
		void getMotorParams(bool* motorEnable, float *speed, float *torque);

		/**
		 * @brief setMotorParams set the motor parameters in the toolbar
		 * @param motorEnable Check to enable motor
		 * @param speed Maximum speed (-5000 rpm to 5000 rpm)
		 * @param torque Maximum torque which can be applied by the motor
		 */
		void setMotorParams(bool motorEnable, float speed, float torque);

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
		virtual void sketchAction(QMouseEvent *event);

		virtual void shapeAction(QTabletEvent *event);
		virtual void shapeAction(QMouseEvent *event);

		virtual void eraseAction(QTabletEvent *event);
		virtual void eraseAction(QMouseEvent *event);

		virtual void selectAction(QTabletEvent *event);
		virtual void selectAction(QMouseEvent *event);

		virtual void connectAction(QTabletEvent *event);
		virtual void connectAction(QMouseEvent *event);

		virtual void searchAction();

		unsigned int getPhysicsMask() const;
		void setPhysicsMask(unsigned int newMask);

		bool isTapOverrideEnabled() const;
		void setTapOverride(bool value);
		void overrideOnTapAndHold(QTapAndHoldGesture* gesture);

	signals:
		void signalSearchComplete();
		void signalSearchBegin();
		void signalSearchItemSelect();
		void signalPhysicsMaskChange();

	public slots:
		/**
		 * @brief Disables the touch input when stylus is near the tablet
		 */
		void stylusEnter();

		/**
		 * @brief Enables the touch input when stylus is near the tablet
		 */
		void stylusLeave();

		/**
		 * @brief When set enables/disables touch input on all the components
		 * @param value
		 */
		void setAcceptComponentTouch(bool value);

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

		/**
		 * @brief onSearchItemSelect is called with the selected search result. It loads/
		 * replaces the selected stroke with the search result
		 * @param result SearchResult selected
		 */
		void onSearchItemSelect(SearchResult* result);

		/**
		 * @brief onSearchItemSelect is called with the graphics item containing search result
		 * details. It loads/replaces the selected stroke with the search result as well as deletes
		 * the display
		 * @param result Selected graphics item
		 */
		void onSearchItemSelect(cdSearchGraphicsItem* result);

		/**
		 * @brief closeSearchResultDisplay closes the display of search results by destroying all
		 * the displayed results as well as enabling touch on all the components (DO NOT IMPLEMENT NOW)
		 */
		void closeSearchResultDisplay();

		/**
		 * @brief Receives tablet/pen event from view
		 * @param event QTabletEvent
		 * @param view QGraphicsView from which the event originiated
		 */
		void onTabletEventFromView(QTabletEvent *event, QGraphicsView *view);

		/**
		 * @brief Receives mouse event from view
		 * @param event QMouseEvent
		 * @param status QGraphicsView from which the event originiated
		 */
		void onMouseEventFromView(QMouseEvent *event, QGraphicsView *view);

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
		 * @brief Receives touch events which were not accepted/processed by the
		 * scene
		 * @param event QTouchEvent pointer from scene
		 */
		bool onTouchEventFromScene(QTouchEvent* event);

		/**
		 * @brief Manages gesture events received from viewport.
		 * @param event QGestureEvent
		 * @return
		 * @remarks Should the events be accepted by Component object in the Scene
		 */
		void onGestureEventFromView(QGestureEvent* event);

		/**
		 * @brief Manages gesture events received from viewport.
		 * @param event QGestureEvent
		 * @return
		 * @remarks Should the events be accepted by Component object in the Scene
		 */
		void onPhysicsMaskUpdate();

		void connectionModeReset();

		void setToEdit();

		void setModeScribble();
		void setModeLockItem();
		void setModeHingeJoint();
		void setModeSliderJoint();
		void setModeSpringJoint();
		void setModeForce();

		void setMode(UI::MODE newMode);

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
		friend class cdState;
		friend class LoadModel;
	};
}

#endif //__PLAYGOCONTROLLER_H__
