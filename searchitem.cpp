#include "searchitem.h"
namespace CDI
{
    SearchItem::SearchItem(QObject *parent, QString file) :
        Item(parent)
    {
        transform = QTransform();
        searchImage = new QPixmap();
        searchImage->load(file);
        origFile = file;
    }

    SearchItem::SearchItem(QObject *parent, QPixmap &pixmap_data) :
        Item(parent)
    {
        transform = QTransform();
        origFile = "";
        searchImage = new QPixmap(pixmap_data);
    }

    SearchItem::~SearchItem()
    {
        if (searchImage != NULL)
            delete searchImage;
    }
}
