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

#include "cdisearchgraphicsitem.h"
#include "graphicspathitem.h"
#include "searchmanager.h"
#include "graphicsitemgroup.h"

#include "page.h"

namespace CDI
{
	class GraphicsPathItem;
	class GraphicsItemGroup;
	class GraphicsPolygon2D;
	/**
	 * @brief The SketchScene class primarily handles the rendering of all the scene objects
	 * and processes some of the events received
	 * It also keeps track of different objects.
	 */
	class SketchScene : public QGraphicsScene
	{
		Q_OBJECT
	public:
		QList<SearchGraphicsItem*> searchResults;

		QList<GraphicsPathItem*> freeStrokes;

		QHash<QUuid, QGraphicsItem*> item_key_hash;

	protected:
		Page* _page;

		SearchManager* searchManager;

	public:
		SketchScene(Page* page, QObject* parent = 0);

		~SketchScene();

		void clear();

		void drawBackground(QPainter * painter, const QRectF & rect);

		Page* page() { return _page; }

		/**
		 * @brief Creates a new component and adds stroke
		 * @param stroke
		 * @return
		 */
		GraphicsItemGroup* addComponent();
		GraphicsPathItem* addStroke(GraphicsItemGroup* parentItem, QColor color = Qt::black, float thickness = 2);
		GraphicsPolygon2D* addPolygon(GraphicsItemGroup* parentItem);

		void insertItem(GraphicsItemGroup* child, GraphicsItemGroup* parent);
		void insertItem(GraphicsPathItem *child, GraphicsItemGroup *parent);
		void insertItem(GraphicsPolygon2D* child, GraphicsItemGroup* parent);

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
		 * @brief getSelectedStrokes returns a list of strokes near a
		 * the given point pos within margin
		 * @param pos point coordinates
		 * @param margin distance from point
		 * @return list of strokes which fall within the range
		 * \todo Order the list in increasing order of distance
		 * @remark Finding the minimum distance from given point might be
		 * expensinve and not implemented now
		 */
		QList<GraphicsPathItem*> getSelectedStrokes(Point2D pos, float margin);

		void SelectSearchResult(SearchGraphicsItem* searchItem);

	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent* mouseEvent);

		void mouseMoveEvent(QGraphicsSceneMouseEvent* mouseEvent) ;

		void mouseReleaseEvent(QGraphicsSceneMouseEvent* mouseEvent) ;

		void searchFromSelection();

	signals:
		void signalMouseEvent(QGraphicsSceneMouseEvent* mouseEvent, int status);

	public slots:
		void OnSearchComplete();

		void onItemDisplayStatusChanged(AbstractModelItem* item);

		void onItemTransformChanged(AbstractModelItem* item);

		void onItemDestroy(AbstractModelItem* item);

		void onItemParentChange(AbstractModelItem* item);

		/////////////////////////////////////////////////////////////

		void onDeleteAllItems(Page* page);

		void onReloadPage(Page* page);

		void onItemRemove(QUuid itemId);

		void onItemUpdate(QUuid itemId);

		void onItemRedraw(QUuid itemId);

		void onItemIdUpdate(QUuid oldID, QUuid newID);

		void onItemAdd(AbstractModelItem* item);

		// Called only from within class
		void onComponentAdd(Component* component);
		void onAssemblyAdd(Assembly* assembly);

		void onComponentMerge(Component* a, Component* b);
		void onAssemblyMerge(Assembly* a, Assembly* b);

		void onAssemblyUpdate(Assembly* assembly);
		void onComponentUpdate(Component* component);

		// Equivalent to deleting object
		void onAssemblyDelete(Assembly* assembly);
		void onComponentDelete(Component* component);

	};
}
