#include "cdisearchgraphicsitem.h"

namespace CDI {
    SearchGraphicsItem::SearchGraphicsItem(QGraphicsItem *parent)
        : QGraphicsPixmapItem(parent)
    {
        _type = IMAGE;
    }

    SearchGraphicsItem::SearchGraphicsItem(
            const QPixmap &pixmap, QString filePath, QGraphicsItem* parent)
        : QGraphicsPixmapItem(pixmap, parent)
    {
        sourceFilePath = filePath;
    }

    Item::Type SearchGraphicsItem::GetType()
    {
        return _type;
    }
}
