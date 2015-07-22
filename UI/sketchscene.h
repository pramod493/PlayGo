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
		Page* _page;

	public:
		SketchScene();

		SketchScene(Page* page);

		~SketchScene();

		void clear();

		Page* page() const;
	};
}
