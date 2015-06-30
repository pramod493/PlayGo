#pragma once
#include <QGraphicsPixmapItem>
#include "searchresult.h"

namespace CDI
{
	class SearchView;
	class GraphicsSearchItem : public QGraphicsPixmapItem
	{
	protected:
		SearchResult* _parentResult;

		SearchView* _searchView;

	public:
		GraphicsSearchItem(SearchResult* searchresult, int dim = 150, QGraphicsItem* parent = NULL);

		QUuid getSearchItemId();

		SearchResult* getSearchResult();

		void setView(SearchView* view);

	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent* event);

		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);
	};
}
