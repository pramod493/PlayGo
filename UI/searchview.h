#pragma once
#include <QGraphicsView>
#include "stroke.h"
#include "searchresult.h"
#include "graphicspathitem.h"
#include "graphicssearchitem.h"

namespace CDI
{
	class SearchView : public QGraphicsView
	{
		Q_OBJECT
	public:
		int imgDim;

		int margin;
	protected:
		QList<GraphicsSearchItem*> _searchDispItems;

		QList<GraphicsPathItem*> _selectedStrokes;

		// When one of the search results are selected,
		// all the strokes are grouped into one Component
		// and an Image component is created. Therefore, for
		// further selections, only update the Image object

	public:
		SearchView(QWidget *parent = 0);

		~SearchView();

		void clear();

	protected:
		void mousePressEvent(QMouseEvent *event);

	public slots:
		void LoadSearchData(QList<GraphicsPathItem *>& selectedStrokes, QList<SearchResult*>& searchResults);

		void onSearchResultSelect(SearchResult* result);
	signals:
		void signalOnSearchResultSelect(SearchResult*);
	};
}
