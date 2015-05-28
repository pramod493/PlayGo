#pragma once
#include <QLinkedList>
#include <QGraphicsScene>
#include <QEvent>
#include <QTabletEvent>
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QTouchEvent>
#include <QGraphicsRectItem>

#include <QList>
#include <QGraphicsSceneMouseEvent>
#include "cdisearchgraphicsitem.h"
#include "cdigraphicspathitem.h"
#include "searchmanager.h"
//#include "pixmapitem.h"
#include "graphicsitemgroup.h"

#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QPen>

#include <Box2D/Box2D.h>

namespace CDI
{
	class SketchScene : public QGraphicsScene
	{
		Q_OBJECT
    public:
        // Physics variables
        b2World* physicsWorld;

        enum MODE {None, Draw, Erase, Transform, Edit, Select, Search};

        QTabletEvent::PointerType pointerType;
        QTabletEvent::TabletDevice tabletDevice;

		// Pen and brush information
        QPen defaultPen;
        QPen highlightPen;
        QPen marqueeSelectPen;
        QBrush defaultBrush;
        QBrush fillBrush;

        int brushWidth;

        bool mouse_mode_enabled;

        QList<SearchGraphicsItem*> searchResults;

        QList<GraphicsPathItem*> freeStrokes;

		QGraphicsPolygonItem* selectionMarquee;
		QPolygonF selectionPolygon;

    protected:


        MODE current_mode;

        bool drawing_right_now;

        QGraphicsPathItem* parent_item;

        GraphicsPathItem* current_stroke;

        SearchManager* searchManager;

	public:
		SketchScene(QObject* parent = 0);

		~SketchScene();

		void drawBackground(QPainter* painter, const QRectF &rect);

        void clear();

        void BrushPress(QPointF scenePos, float pressure = 1.0);

        void BrushMove(QPointF scenePos, float pressure = 1.0);

        void BrushRelease(QPointF scenePos, float pressure = 1.0);

        void SelectSearchResult(SearchGraphicsItem* searchItem);

        void SaveScene(QString file);

		SketchScene* Clone();

    protected:
        //// NOTE - Does the mouse event need to be passed on the base class event handler?
        void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);

        void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) ;

        void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) ;

        void DrawAction(QTabletEvent *event, QPointF scenePos);

        void EraseAction(QTabletEvent *event, QPointF scenePos);

        void SelectAction(QTabletEvent *event, QPointF scenePos);

	signals:
        void signalModeChanged(MODE newMode);

        void signalBrushReleased(SketchScene* scene);

    private slots:
        void OnBrushRelease();

	public slots:        
        void slotTabletEvent(QTabletEvent *event, QPointF scenePos);

		void OnSearchTrigger();

        void OnSearchComplete();

		inline void setBrushWidth(int size);

		inline void setBrushColor(QString name, QColor color);

		inline void slotSetSceneMode(MODE newMode);

		inline void setToNone();

		inline void setToDraw();

		inline void setToErase();

		inline void setToTransform();

		inline void setToEdit();

		inline void setToSelect();

        // NOTE - THis one makes no sense since this is an one time event
        // Similar to play/pause/reset event which occur only once.
		inline void setToSearch();

		void PhysicsStep();

	};

	/*******************************************
	 * inline functions
	 * ***************************************/

	inline void SketchScene::setBrushWidth(int size)
	{
		defaultPen.setWidth(size);
		highlightPen.setWidth(size+3);
	}

	inline void SketchScene::setBrushColor(QString name, QColor color)
	{
		defaultPen.setColor(color);
	}

	inline void SketchScene::slotSetSceneMode(MODE newMode)
	{
		if (current_mode == newMode) return;
		current_mode = newMode;
		emit signalModeChanged(current_mode);
	}

	inline void SketchScene::setToNone()
	{
		current_mode = None;
		emit signalModeChanged(current_mode);
	}

	inline void SketchScene::setToDraw()
	{
		current_mode = Draw;
		emit signalModeChanged(current_mode);
	}

	inline void SketchScene::setToErase()
	{
		current_mode = Erase;
		emit signalModeChanged(current_mode);
	}

	inline void SketchScene::setToTransform()
	{
		current_mode = Transform;
		emit signalModeChanged(current_mode);
	}

	inline void SketchScene::setToEdit()
	{
		current_mode = Edit;
		emit signalModeChanged(current_mode);
	}

	inline void SketchScene::setToSelect()
	{
		current_mode = Select;
		emit signalModeChanged(current_mode);
	}

	inline void SketchScene::setToSearch()
	{
		current_mode = Search;
	}
}
