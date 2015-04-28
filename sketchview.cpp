#include "sketchview.h"

namespace CDI
{
	SketchView::SketchView(QWidget* parent)
		: QGraphicsView(parent)
	{
		setAttribute(Qt::WA_AcceptTouchEvents, true);
		QLinearGradient gradient = QLinearGradient(0,0,0,1000);
		gradient.setColorAt(0.0,Qt::white);
		gradient.setColorAt(1.0, QColor(200,200,200,255));
		setBackgroundBrush(QBrush(gradient));
	}

	SketchView::~SketchView()
	{

	}

	// Handle the resizing of the main window
	void SketchView::resizeEvent(QResizeEvent *event)
	{
//		scene->setSceneRect(QRect(QPoint(), event->size()));
		QGraphicsView::resizeEvent(event);
	}

	// Override touch event handling
	bool SketchView::event(QEvent *event)
	{
		switch (event->type())
		{
			case QEvent::TouchEnd :
			case QEvent::TouchUpdate :
			case QEvent::TouchBegin :
			{
				event->ignore();
				return true;
			}
		}

		return QGraphicsView::event(event);
	}

	// Override stylus event handling
	void SketchView::tabletEvent(QTabletEvent *event)
	{
		switch (event->type())
		{
			case QEvent::TabletPress :
			case QEvent::TabletMove :
			case QEvent::TabletRelease :
			{
				QPointF scenePos = mapToScene(event->pos());
				emit signalViewTabletEvent(event, scenePos);
				event->accept();
				break;
			}
		}
		update();
	}

}
