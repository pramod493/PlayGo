#include "searchview.h"
#include <QtAlgorithms>
#include "QsLog.h"
#include "searchresult.h"

namespace CDI
{
	SearchView::SearchView(QWidget *parent)
		: QGraphicsView(parent)
	{
		QGraphicsScene* tmp = new QGraphicsScene(this);
		setScene(tmp);

		imgDim = 150;
		margin = 10;
	}

	SearchView::~SearchView()
	{
		if (scene() != NULL)
			delete scene();
	}

	void SearchView::clear()
	{
		_selectedStrokes.clear();

		if (_searchDispItems.size())
			qDeleteAll(_searchDispItems);
		_searchDispItems.clear();
	}

	void SearchView::LoadSearchData
	(QList<GraphicsPathItem *> &selectedStrokes, QList<SearchResult *> &searchResults)
	{
		clear();
		_selectedStrokes = selectedStrokes;

		for (int i=0; i< searchResults.size(); i++)
		{
			SearchResult* searchresult = searchResults[i];
			GraphicsSearchItem* searchItem = new GraphicsSearchItem(searchresult, imgDim);
			scene()->addItem(searchItem);
			_searchDispItems.push_back(searchItem);
		}
		//graphicsView->mapToScene(graphicsView->viewport()->geometry()).boundingRect()
		QRectF sceneRect = mapToScene(viewport()->geometry()).boundingRect();
		QLOG_INFO() << "Search view rect" << sceneRect;
	}
}
