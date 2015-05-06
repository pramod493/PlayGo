#pragma once
#include "cdicommon.h"
#include <QPixmap>
#include <QFile>
#include <QString>
#include <QTransform>

namespace CDI
{
    class SearchItem : public Item
    {
        Q_OBJECT
    public:
        QTransform transform;

        QPixmap* searchImage;

        QString origFile;

        SearchItem(QObject* parent, QString file);

        SearchItem(QObject* parent, QPixmap& pixmap_data);

        ~SearchItem();

        Type GetType(){return Type::IMAGE;}

        void update() {emit OnUpdate(this);}

    signals:
        void OnUpdate(SearchItem*);
    };
}
