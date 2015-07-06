#pragma once

#include <QObject>
#include <QString>
#include <QGraphicsPixmapItem>
#include <QEvent>
#include <QTouchEvent>

// TODO - SearchItem from the core library is not really useful in here because
// we are trying to ues the search result as a full component. Therefore it would
// be better suited to parent it to something else

namespace CDI
{
    class PixmapItem : public QGraphicsPixmapItem
    {
        Q_OBJECT
    public:
        QString filePath;
    protected:
    public:
        PixmapItem(const QPixmap& pixmap, QString pixmapFile, QGraphicsItem* gparent, QObject* oparent);

//        Item::Type GetType() { return _type; }

        bool sceneEvent(QEvent* event);

    };
}
