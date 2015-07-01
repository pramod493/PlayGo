#include "searchview.h"
#include <QtAlgorithms>
#include "QsLog.h"
#include "searchresult.h"
#include <QMessageBox>

namespace CDI
{
	SearchView::SearchView(QWidget *parent)
		: QGraphicsView(parent)
	{
		QGraphicsScene* tmp = new QGraphicsScene(this);
		setScene(tmp);

		imgDim = 100;
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

		QMessageBox::about(this, "SearchView::clear()", "All items cleared");
	}

	void SearchView::mousePressEvent(QMouseEvent *event)
	{
		//QMessageBox::about(this, QString("Search view"), QString("Search view"));
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
			// Set current view as parent view in the object
			searchItem->setView(this);
			_searchDispItems.push_back(searchItem);
		}
		//graphicsView->mapToScene(graphicsView->viewport()->geometry()).boundingRect()
		QRectF sceneRect = mapToScene(viewport()->geometry()).boundingRect();
		for (int i=0; i< _searchDispItems.size(); i++)
		{
			GraphicsSearchItem* searchItem = _searchDispItems[i];
			searchItem->setPos((i%5) * (imgDim+margin),
						(i/5) * (imgDim+margin));

		}
		QLOG_INFO() << "Search view rect" << sceneRect;
	}

	void SearchView::onSearchResultSelect(SearchResult* result)
	{
		QMessageBox::about(this, "item click", result->resultFilePath);
		emit signalOnSearchResultSelect(result);
	}
}
