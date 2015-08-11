#pragma once
#include <QLinkedList>
#include <QGraphicsScene>
#include <QEvent>
#include <QGraphicsRectItem>
#include <QImage>
#include <QList>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QGraphicsPolygonItem>
#include <QPolygonF>

#include "commonfunctions.h"
#include "abstractmodelitem.h"

#include "stroke.h"
#include "searchmanager.h"
#include "component.h"

#include "page.h"
#include "graphicssearchitem.h"

namespace CDI
{
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
