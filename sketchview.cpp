#include "sketchview.h"
#include <QDebug>

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
				event->setAccepted(true);
                QTouchEvent* e = static_cast<QTouchEvent*>(event);
                qDebug() << "Touch event received at " << e->touchPoints().count() << "places";
				if (e->touchPoints().count() == 3)
				{
					// Pan the scene view
					QList<QTouchEvent::TouchPoint> touchPoints = e->touchPoints();
					qDebug() << touchPoints[0].pos() << touchPoints[1].pos() << touchPoints[2].pos();

				}
				//return true;
            }
        }

        return QGraphicsView::event(event);
    }

    // Override stylus event handling
    void SketchView::tabletEvent(QTabletEvent *event)
    {
        event->accept();
        switch (event->type())
        {
            case QEvent::TabletPress :
            case QEvent::TabletMove :
            case QEvent::TabletRelease :
            {

                QPointF scenePos = mapToScene(event->pos());
                emit signalViewTabletEvent(event, scenePos);
                break;
            }
        }
    }

}
