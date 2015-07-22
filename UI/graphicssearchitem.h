#pragma once
#include <QGraphicsPixmapItem>
#include "searchresult.h"
#include "commonfunctions.h"
#include "cdi2qtwrapper.h"
#include <QEvent>
#include <QTouchEvent>
#include <QDrag>
#include <QMimeData>
#include <QByteArray>

namespace CDI
{
	class SearchView;

	/**
	 * @brief The GraphicsSearchItem class display search results in scene
	 * It loads the image based on value contained in SearchResult object
	 */
	class GraphicsSearchItem : public QGraphicsPixmapItem
	{
	public:
		enum {Type = UserType + SEARCHITEMVIEW };

	protected:
		SearchResult* _parentResult;

		SearchView* _searchView;

		QUuid _id;

	public:
		GraphicsSearchItem(SearchResult* searchresult, int dim = 150, QGraphicsItem* parent = 0);

		virtual ~GraphicsSearchItem();

		int type() const { return Type; }

		QUuid id() const { return _id; }

		SearchResult* getSearchResult();

		void setView(SearchView* view);

		virtual QDataStream& serialize(QDataStream& stream) const;

		virtual QDataStream& deserialize(QDataStream& stream);

	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent* event);

		void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event);

		bool sceneEvent(QEvent *event);
	};
}
