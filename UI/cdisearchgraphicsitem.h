#pragma once

#include <QGraphicsPixmapItem>
#include <QEvent>
#include <QTouchEvent>
#include <QTime>
#include "commonfunctions.h"

namespace CDI
{
	class SearchGraphicsItem : public QGraphicsPixmapItem
    {
    public:
        QString sourceFilePath;

    protected:

        bool touchOn;

        int waitInMiliseconds;

        QTime t;
    public:
        SearchGraphicsItem(QGraphicsItem* parent = 0);

        SearchGraphicsItem(const QPixmap &pixmap, QString filePath, QGraphicsItem* parent = 0);

		bool sceneEvent(QEvent *event);
    };
}
