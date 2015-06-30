#pragma once
#include <QGraphicsPixmapItem>
#include "searchresult.h"

namespace CDI
{
	class GraphicsSearchItem : public QGraphicsPixmapItem
	{
	protected:
		SearchResult* _parentResult;

	public:
		GraphicsSearchItem(SearchResult* searchresult, int dim = 150, QGraphicsItem* parent = NULL);

		QUuid getSearchItemId();

		SearchResult* getSearchResult();
	};
}
