#pragma once

#include <QGraphicsPixmapItem>
#include "cdicommon.h"

namespace CDI
{
    class SearchGraphicsItem : public QGraphicsPixmapItem, public Item
    {
    protected:
        Item::Type _type;
        QString sourceFilePath;
    public:
        SearchGraphicsItem(QGraphicsItem* parent = 0);

        SearchGraphicsItem(const QPixmap &pixmap, QString filePath, QGraphicsItem* parent = 0);

        Item::Type GetType();
    };
}
