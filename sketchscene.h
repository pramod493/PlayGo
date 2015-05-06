#pragma once
#include <QLinkedList>
#include <QGraphicsScene>
#include <QEvent>
#include <QTabletEvent>
#include <QPen>
#include <QBrush>
#include <QPainter>
#include <QTouchEvent>

#include <QList>
#include <QGraphicsSceneMouseEvent>
#include "cdisearchgraphicsitem.h"
#include "cdigraphicspathitem.h"
#include "searchmanager.h"

namespace CDI
{
	class SketchScene : public QGraphicsScene
	{
		Q_OBJECT
    public:
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

    protected:
        QList<SearchGraphicsItem*> searchResults;

        QList<GraphicsPathItem*> freeStrokes;

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

        void BrushPress(QPointF scenePos);

        void BrushMove(QPointF scenePos);

        void BrushRelease(QPointF scenePos);

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

        void OnSearchComplete();

        void setBrushWidth(int size);

        void setBrushColor(QString name, QColor color);

        void slotSetSceneMode(MODE newMode);

        void setToNone();

        void setToDraw();

        void setToErase();

        void setToTransform();

        void setToEdit();

        void setToSelect();

        // NOTE - THis one makes no sense since this is an one time event
        // Similar to play/pause/reset event which occur only once.
        void setToSearch();

	};
}
