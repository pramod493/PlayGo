#include "graphicssearchitem.h"
#include "QsLog.h"
#include "searchview.h"

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

		_searchView = NULL;
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

	void GraphicsSearchItem::setView(SearchView *view)
	{
		_searchView = view;
	}

	void GraphicsSearchItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
	{
		QLOG_INFO() << "Search result clicked" << _parentResult->resultFilePath;
		if (_searchView != NULL)
		{
			_searchView->onSearchResultSelect(_parentResult);
		}
		QGraphicsPixmapItem::mousePressEvent(event);
	}

	void GraphicsSearchItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
	{
		QLOG_INFO() << "Search result double-clicked" << _parentResult->resultFilePath;
		if (_searchView != NULL)
		{
			_searchView->onSearchResultSelect(_parentResult);
		}
		QGraphicsPixmapItem::mouseDoubleClickEvent(event);
	}
}
