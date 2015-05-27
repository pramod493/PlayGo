#include "sketchview.h"
#include <QDebug>

namespace CDI
{
    SketchView::SketchView(QWidget* parent)
        : QGraphicsView(parent)
    {
        setAttribute(Qt::WA_AcceptTouchEvents, true);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        QLinearGradient gradient = QLinearGradient(0,0,0,1000);
        gradient.setColorAt(0.0,Qt::white);
//        gradient.setColorAt(1.0, QColor(200,200,200,255));
        gradient.setColorAt(1.0, QColor(255,250,250,255));
        setBackgroundBrush(QBrush(gradient));

    }

    SketchView::~SketchView()
    {
    }

    // Handle the resizing of the main window
    void SketchView::resizeEvent(QResizeEvent *event)
    {
        //		scene->setSceneRect(QRect(QPoint(), event->size()));
        if (scene() != NULL)
        {
            scene()->setSceneRect(QRect(QPoint(), event->size()));
        }
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
                // Umm... Looks like a touch event. Lets see if we need to do something
                QTouchEvent* touchEvent = static_cast<QTouchEvent*>(event);
                qDebug() << "Touch:Pre:Points" << touchEvent->touchPoints().count() << "Pre-send";
                bool eventStatus = QGraphicsView::event(event);
                bool isAccepted = event->isAccepted();
                event->setAccepted(false);
                qDebug()  << "Touch:Pre:Points" << touchEvent->touchPoints().count() <<
                             "return:" << eventStatus << "accept:" << isAccepted;
                return false;

                if (touchEvent->touchPoints().count() == 2 && scene() != NULL)
				{
                    const QTouchEvent::TouchPoint &touchPoint1 = touchEvent->touchPoints().first();
                    const QTouchEvent::TouchPoint &touchPoint2 = touchEvent->touchPoints().last();
                    QLineF line1(touchPoint1.lastScenePos(), touchPoint2.lastScenePos());
                    QLineF line2(touchPoint1.scenePos(), touchPoint2.scenePos());
                    qreal p_scale = line2.length()/line1.length();
                    scale(p_scale,p_scale);
                    qDebug() << "View scaling" << p_scale;
                    // We did it. not sure how it's going to go down
				}
                return true;
                break;
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
//                Test scaling in QGraphicsView
//                Can we also try rotation?
//                if (event->pointerType()== QTabletEvent::Eraser)
//                    scale(1.05,1.05);
//                if (event->pointerType() == QTabletEvent::Pen)
//                    scale (0.95,0.95 );
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
