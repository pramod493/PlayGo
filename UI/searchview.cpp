#include "searchview.h"
#include <QtAlgorithms>

namespace CDI
{
	SearchView::SearchView(QWidget *parent)
		: QGraphicsView(parent)
	{
		QGraphicsScene* tmp = new QGraphicsScene(this);
		setScene(tmp);
	}

	SearchView::~SearchView()
	{
		if (scene() != NULL)
			delete scene();
	}

	void SearchView::LoadSearchData
	(QList<Stroke*> selectedStrokes, QList<SearchResult*> searchResults)
	{
		// TODO
		_selectedStrokes.clear();
		_selectedStrokes = selectedStrokes;

		if (_searchDispItems.size())
			qDeleteAll(_searchDispItems);
		_searchDispItems.clear();

		for (int i=0; i< searchResults.size(); i++)
		{
			GraphicsSearchItem* searchItem = new GraphicsSearchItem(searchResults[i]);
			scene()->addItem(searchItem);
			_searchDispItems.push_back(searchItem);
		}

		// todo - layout objects in scene
	}
}
