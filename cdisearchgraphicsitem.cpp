#include "cdisearchgraphicsitem.h"

namespace CDI {
    CDISearchGraphicsItem::CDISearchGraphicsItem(QGraphicsItem *parent)
        : QGraphicsPixmapItem(parent)
    {

    }

    CDISearchGraphicsItem::CDISearchGraphicsItem(
            const QPixmap &pixmap, QString filePath, QGraphicsItem* parent)
        : QGraphicsPixmapItem(pixmap, parent)
    {
        sourceFilePath = filePath;
    }
}
