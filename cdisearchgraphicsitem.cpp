#include "cdisearchgraphicsitem.h"
#include <QGraphicsScene>
#include <QDebug>
#include "sketchscene.h"
#include "pixmapitem.h"

namespace CDI {
	SearchGraphicsItem::SearchGraphicsItem(QGraphicsItem *parent)
		: QGraphicsPixmapItem(parent)
	{
		_type = IMAGE;
		setAcceptDrops(true);
		setAcceptTouchEvents(true);

        setShapeMode(QGraphicsPixmapItem::BoundingRectShape);

        touchOn = false;
        waitInMiliseconds = 500;
        t = QTime();
	}

	SearchGraphicsItem::SearchGraphicsItem(
			const QPixmap &pixmap, QString filePath, QGraphicsItem* parent)
		: QGraphicsPixmapItem(pixmap, parent)
	{
		sourceFilePath = filePath;
		setAcceptDrops(true);
		setAcceptTouchEvents(true);

        setShapeMode(QGraphicsPixmapItem::BoundingRectShape);

        touchOn = false;
        waitInMiliseconds = 500;
        t = QTime();
	}

	Item::Type SearchGraphicsItem::GetType()
	{
		return _type;
	}

	bool SearchGraphicsItem::sceneEvent(QEvent* event)
	{
        //// Tap to select item
        switch (event->type()) {
            case QEvent::TouchEnd :
            case QEvent::TouchUpdate :
            case QEvent::TouchBegin :
                event->setAccepted(true);
                QTouchEvent* e = static_cast<QTouchEvent*> (event);
                if (e->touchPoints().size() != 1) return true;  // Only single touch allowed
                    break;
        }
        switch (event->type())
        {
            case QEvent::TouchBegin :
                touchOn = true;
                t.restart();
                break;
            case QEvent::TouchUpdate :
                break;
            case QEvent::TouchEnd :
                if (touchOn && (t.elapsed() < waitInMiliseconds))
                {
                    touchOn = false;
                    SketchScene* parentScene = static_cast<SketchScene*>(scene());  // NOTE - Do we really need the SketchScene?
                    PixmapItem* pixItem =new PixmapItem(QPixmap(sourceFilePath), sourceFilePath, NULL, NULL);
                    pixItem->setFlag(QGraphicsItem::ItemIsMovable, true);
                    parentScene->addItem(pixItem);
                }
                break;
        }

		return true;
	}
}
