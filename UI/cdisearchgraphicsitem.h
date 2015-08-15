#pragma once
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QEvent>
#include <QTouchEvent>
#include <QTime>
#include <QObject>
#include "commonfunctions.h"
#include "searchresult.h"

namespace CDI
{
	class cdSearchGraphicsItem : public QObject, public QGraphicsRectItem
	{
		Q_OBJECT
	protected:
		bool touchOn;			/**< Enabled when it receives a touch/mouse input */

		int waitInMiliseconds;	/**< Do not accept selection if longer than this time */

		QTime t;

		SearchResult result;	/**< Search result stored */

	public:
		/**
		 * @brief cdSearchGraphicsItem creates a new Search Item which can be selected
		 * @param searchResult Search Result to be displayed
		 * @param dim	Display dimensions (in pixels)
		 * @param parent parent QGraphicsItem
		 */
		cdSearchGraphicsItem(SearchResult* searchResult, int dim = 150, QGraphicsItem* parent = 0);

		virtual ~cdSearchGraphicsItem() {}

		/**
		 * @brief Manage the mosue/stylus event to the result
		 * @param event Event sent to item
		 * @return true if accepted
		 */
		bool sceneEvent(QEvent *event);

		/**
		 * @brief Returns the search result associated with the item
		 * @return SearchResult
		 */
		SearchResult getResult() const;

	signals:
		/**
		 * @brief signalSearchItemSelected is emitted when search result is selected
		 */
		void signalSearchItemSelected(cdSearchGraphicsItem*);
	};
}
