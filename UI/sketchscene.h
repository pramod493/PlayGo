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
	 * and processes some of the events received
	 * It also keeps track of different objects.
	 */
	class SketchScene : public QGraphicsScene
	{
		Q_OBJECT
	public:

	protected:
		Page* _page;

	public:
		SketchScene(Page* page, QObject* parent = 0);

		~SketchScene();

		void clear();

		Page* page() { return _page; }

		/**
		 * @brief getSelectionImage retuns cropped image containing all the free
		 * strokes contained within the given polygon
		 * @param polygon: Masking polygon. Everything outside this will be removed
		 * @return Snapshot of selection
		 */
		QImage getSelectionImage(QPolygonF polygon);

		/**
		 * @brief getSelectionImage returns the cropped image containing all
		 * the free strokes in the component
		 * @return QImage of all the free strokes
		 */
		QImage getSelectionImage();

        /**
         * @brief Create Image based on strokes which are highlighted
         * @return Image containing selected strokes
         * @todo Allow Images to be selected as well.
         */
        QImage getSelectionImageFromHighlight();

		/**
		 * @brief getSelectedStrokes returns a list of strokes near a
		 * the given point pos within margin
		 * @param pos point coordinates
		 * @param margin distance from point
		 * @return list of strokes which fall within the range
		 * \todo Order the list in increasing order of distance
		 * @remark Finding the minimum distance from given point might be
		 * expensinve and not implemented now
		 */
		QList<Stroke*> getSelectedStrokes(Point2D pos, float margin);

        /**
         * @brief Checks if a given stroke is contained within the given polygon
         * @param selectionPolygon Envelope polygon
         * @param minimalAllowedSelection Sets the minimum length of strokes which need to \
         * be within the given polygon
         * @return List of strokes contained within the polygon region
         */
		QList<Stroke*> getSelectedStrokes(QPolygonF selectionPolygon, float minimalAllowedSelection);

		/**
		 * @brief Returns list of highlighted strokes
		 * @return
		 */
		QList<Stroke*> getHighlightedStrokes();

		/**
		 * @brief Get all the selected items
		 * @return List of selected items
		 */
		QList<QGraphicsItem*> getSelectedItems() { return QList<QGraphicsItem*>(); }

		/**
		 * @brief Get all the highlighted items
		 * @return List of highlighted items
		 */
		QList<QGraphicsItem*> getHighlightedItems() { return QList<QGraphicsItem*>(); }

		QRectF getBoundingBox(QList<QGraphicsItem*> listOfItems);

	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);

		void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) ;

		void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) ;

	signals:
		void signalMouseEvent(QGraphicsSceneMouseEvent* mouseEvent, int status);

	public slots:
		void clearStrokeHighlight();
	};
}
