#pragma once

#include <QGraphicsScene>
#include <QEvent>
#include <QTabletEvent>
#include <QPen>
#include <QBrush>

#include <QList>
#include <QGraphicsSceneMouseEvent>
#include "cdisearchgraphicsitem.h"
#include "cdigraphicspathitem.h"

namespace CDI
{
	class SketchScene : public QGraphicsScene
	{
		Q_OBJECT
    public:
        enum MODE {None, Draw, Erase, Transform, Edit};

        QTabletEvent::PointerType pointerType;
        QTabletEvent::TabletDevice tabletDevice;

		// Pen and brush information
        QPen defaultPen;
        QPen highlightPen;
        QPen marqueeSelectPen;
        QBrush defaultBrush;
        QBrush fillBrush;

        bool mouse_mode_enabled;

    protected:
        QList<CDISearchGraphicsItem*> searchResults;

        QList<CDIGraphicsPathItem*> freeStrokes;

        MODE current_mode;

        bool drawing_right_now;

        QGraphicsPathItem* parent_item;

        CDIGraphicsPathItem* current_stroke;

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
		void ModeChanged(MODE newMode);

	public slots:
        void slotTabletEvent(QTabletEvent *event, QPointF scenePos);

        void slotSetSceneMode(MODE newMode);
	};
}
