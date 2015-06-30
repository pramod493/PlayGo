#include "graphicssearchitem.h"

namespace CDI
{
	GraphicsSearchItem::GraphicsSearchItem
	(SearchResult* searchresult, int dim, QGraphicsItem* parent)
		: QGraphicsPixmapItem(parent)
	{
		if (dim <= 0) dim = 150;
		_parentResult = searchresult;
		QPixmap pixmap = QPixmap();
		pixmap.load(_parentResult->resultFilePath);
		pixmap = pixmap.scaled(QSize(dim, dim), Qt::KeepAspectRatio);
		setPixmap(pixmap);
	}

	QUuid GraphicsSearchItem::getSearchItemId()
	{
		if (_parentResult== NULL) return QUuid();	// Returns an invalid id
		return _parentResult->id();
	}

	SearchResult* GraphicsSearchItem::getSearchResult()
	{
		return _parentResult;
	}
}
