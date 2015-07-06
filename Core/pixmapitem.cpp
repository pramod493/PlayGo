#include "pixmapitem.h"
#include <QDebug>

namespace CDI
{
    PixmapItem::PixmapItem(const QPixmap& pixmap, QString pixmapFile, QGraphicsItem* gparent, QObject* oparent) :
        QGraphicsPixmapItem(pixmap, gparent), Item(oparent), filePath(pixmapFile), _type(IMAGE)
    {
        setAcceptDrops(true);
        setAcceptTouchEvents(true);

        setShapeMode(QGraphicsPixmapItem::MaskShape);
//        setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    }

    bool PixmapItem::sceneEvent(QEvent* event)
    {
        switch (event->type())
        {
            case QEvent::TouchBegin:
            case QEvent::TouchUpdate:
            case QEvent::TouchEnd:
            {
                event->accept();
                qDebug() << "pixmap accept touch";
                QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);

                if (touchEvent->touchPoints().count() == 2) {
                    const QTouchEvent::TouchPoint &touchPoint1 = touchEvent->touchPoints().first();
                    const QTouchEvent::TouchPoint &touchPoint2 = touchEvent->touchPoints().last();

                    QLineF line1(touchPoint1.lastScenePos(), touchPoint2.lastScenePos());
                    QLineF line2(touchPoint1.scenePos(), touchPoint2.scenePos());

                    setTransformOriginPoint(line2.pointAt(0.5));

                    qreal p_rotation = rotation() + line2.angleTo(line1);
                    qreal p_scale = scale() * line2.length()/line1.length();
                    if (p_scale > 3.0) p_scale = 3.0;
//                    if (p_rotation > 360.0) p_rotation = 360;
//                    if (p_rotation < -360.0) p_rotation = -360;
                    setRotation(p_rotation);
                    setScale(p_scale);
                }
                if (touchEvent->touchPoints().count() == 1)
                {
                    const QTouchEvent::TouchPoint &touchPoint = touchEvent->touchPoints().first();
                    QPointF dist = touchPoint.scenePos() - touchPoint.lastScenePos();
                    moveBy(dist.x(),dist.y());
                }
                return true;
                break;
            }
            case QEvent::TouchCancel:
                qDebug() << "Touch cancel";
                break;
        }
        return QGraphicsPixmapItem::sceneEvent(event);
    }
}
