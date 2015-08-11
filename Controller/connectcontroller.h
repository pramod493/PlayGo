#ifndef __CDI_CONNECT_CONTROLLER__
#define __CDI_CONNECT_CONTROLLER__
#include <QGesture>
#include <QGestureEvent>
#include "playgocore.h"	// includes all the core libraries
#include <QToolBar>
#include "pdollarrecognizer.h"
#include "penstroke.h"

class QGraphicsItem;
class QTabletEvent;
class QGesture;
// try to code without using QGraphicsscene
namespace CDI
{
	class CDIWindow;
	class SketchScene;
	class SketchView;	
	class PlayGoController;

	class ConnectController : public QObject
	{
		Q_OBJECT
	protected:
		Page* _page;
		SketchScene* _scene;
		SketchView* _view;
		PlayGoController* _parentController;

		CDIWindow* _mainWindow;
		QToolBar* connectionOptionsToolbar;

		enum ConnectionMode {GestureSketch, HingeJoint, SpringJoint, ApplyForce};	// Leave it to be private

		ConnectionMode _currentMode;

		PDollarRecognizer *sketchRecognizer;

		QList<PenStroke*> _tmpStrokes;

		PenStroke* _currentStroke;

	public:
		explicit ConnectController(PlayGoController *parent = 0);

		virtual ~ConnectController();

		virtual void onPress(QPointF scenePos);
		virtual void onMove(QPointF scenePos);
		virtual void onRelease(QPointF scenePos);

		void showToolbar();

		void hideToolbar();

	protected:
		virtual void createToolbar();

	signals:

	public slots:
		void onGestureReceive(QGraphicsItem* graphicsitem);
		void onTabletEvent(QTabletEvent* event);
		void onMouseEventFromScene(QGraphicsSceneMouseEvent* event, int eventStatus);

		void setModeScribble();
		void setModeHingeJoint();
		void setModeSpringJoint();
		void setModeForce();

		void onGestureRecognized(QString gestureName, float gestureScore);

		void clean();
	};
}

#endif //__CDI_CONNECT_CONTROLLER__
