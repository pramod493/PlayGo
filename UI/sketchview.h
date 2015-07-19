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

		void drawForeground(QPainter * painter, const QRectF & rect);

		SketchScene* getSketchScene();

	protected:
		void resizeEvent(QResizeEvent *event);

		//bool event(QEvent* event);

		//void tabletEvent(QTabletEvent *event);

		void wheelEvent(QWheelEvent* event);

		void dragEnterEvent(QDragEnterEvent * event);

		void dragLeaveEvent(QDragLeaveEvent * event);

		void dragMoveEvent(QDragMoveEvent * event);

		void dropEvent(QDropEvent *event);

	signals:
		/**
		 * @brief Connect with appropriate function in order to draw on background
		 * @param painter
		 * @param rect
		 */
		void viewDrawbackground(QPainter* painter, const QRectF & rect);

		void viewDrawforeground(QPainter* painter, const QRectF & rect);

		void viewImageDrop(QString imagepath, QObject* view, QDropEvent* event);
	};
}
