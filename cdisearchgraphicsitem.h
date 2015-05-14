#pragma once

#include <QGraphicsPixmapItem>
#include <QEvent>
#include <QTouchEvent>
#include <QTime>

#include "cdicommon.h"

namespace CDI
{
    class SearchGraphicsItem : public QGraphicsPixmapItem, public Item
    {
    public:
        QString sourceFilePath;

    protected:
        Item::Type _type;

        bool touchOn;

        int waitInMiliseconds;

        QTime t;
    public:
        SearchGraphicsItem(QGraphicsItem* parent = 0);

        SearchGraphicsItem(const QPixmap &pixmap, QString filePath, QGraphicsItem* parent = 0);

        Item::Type GetType();

		bool sceneEvent(QEvent *event);
    };
}
