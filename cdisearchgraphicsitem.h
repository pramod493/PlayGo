#pragma once

#include <QGraphicsPixmapItem>

namespace CDI
{
    class CDISearchGraphicsItem : public QGraphicsPixmapItem
    {
        QString sourceFilePath;
    public:
        CDISearchGraphicsItem(QGraphicsItem* parent = 0);

        CDISearchGraphicsItem(const QPixmap &pixmap, QString filePath, QGraphicsItem* parent = 0);
    };
}
