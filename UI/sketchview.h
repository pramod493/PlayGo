#pragma once
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTabletEvent>
#include <QTouchEvent>
#include <QWheelEvent>
#include "sketchscene.h"

#include "page.h"

namespace CDI
{
	class SketchView : public QGraphicsView
	{
		Q_OBJECT
	protected:
		Page* _page;

		SketchScene* _scene;
	public:
		SketchView(Page* page, QWidget* parent = 0);

		virtual ~SketchView();

		void drawBackground(QPainter * painter, const QRectF & rect);

		SketchScene* getSketchScene();

	protected:
		void resizeEvent(QResizeEvent *event);

		bool event(QEvent* event);

		void tabletEvent(QTabletEvent *event);

		void wheelEvent(QWheelEvent* event);

	signals:
		void viewTabletEvent(QTabletEvent *tabletEvent, QGraphicsView *view);

		void viewTouchEvent(QTouchEvent *touchEvent, QGraphicsView *view);
	};
}
