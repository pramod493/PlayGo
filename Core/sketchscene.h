#pragma once

#include <QGraphicsScene>
#include "commonfunctions.h"

class QEvent;
class QTouchEvent;
namespace CDI
{
	class Page;
	/**
	 * @brief The SketchScene class primarily handles the rendering of all the scene objects
	 */
	class SketchScene : public QGraphicsScene
	{
		Q_OBJECT
	protected:
		/**
		 * @brief Reference to parent page
		 */
		Page* _page;

	public:
		SketchScene();

		SketchScene(Page* page);

		/**
		 * @brief Deletes the scene and removes all the components
		 */
		virtual ~SketchScene();

		void clear();

		/**
		 * @brief Override the default behavior of scene events
		 * @param sceneEvent
		 * @return True, if accepted; false otherwise
		 */
		bool event(QEvent* sceneEvent);

		/**
		 * @brief Returns the reference to parent Page
		 * @return
		 */
		Page* page() const;

	signals:
		void signalUnacceptedTouchEvent(QTouchEvent* event);

	};
}
