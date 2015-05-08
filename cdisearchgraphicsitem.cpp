#include "cdisearchgraphicsitem.h"
#include <QDebug>
#include <QEvent>

namespace CDI {
	SearchGraphicsItem::SearchGraphicsItem(QGraphicsItem *parent)
		: QGraphicsPixmapItem(parent)
	{
		_type = IMAGE;
		setAcceptDrops(true);
		setAcceptTouchEvents(true);
	}

	SearchGraphicsItem::SearchGraphicsItem(
			const QPixmap &pixmap, QString filePath, QGraphicsItem* parent)
		: QGraphicsPixmapItem(pixmap, parent)
	{
		sourceFilePath = filePath;
		setAcceptDrops(true);
		setAcceptTouchEvents(true);
	}

	Item::Type SearchGraphicsItem::GetType()
	{
		return _type;
	}

	bool SearchGraphicsItem::sceneEvent(QEvent* event)
	{
		qDebug() << "Event received";
		event->setAccepted(true);

		switch (event->type()) {
		case QEvent::TouchEnd :
		case QEvent::TouchUpdate :
		case QEvent::TouchBegin :
			qDebug () << "Touch event received on graphicsItem";
			break;
		}
		return true;
	}
}
