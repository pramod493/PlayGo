#include "sketchscene.h"
#include <QtAlgorithms>
#include <QList>
#include "QsLog.h"
#include "graphicsitemgroup.h"
#include "graphicspathitem.h"
#include "graphicspolygon2d.h"
#include "graphicspixmap.h"

namespace CDI
{
	SketchScene::SketchScene(Page *page, QObject* parent) :
		QGraphicsScene(parent)
	{
		_page = page;
		searchResults = QList<SearchGraphicsItem*>();
		freeStrokes = QList<GraphicsPathItem*>();
		// searchManager = new SearchManager(this);
		searchManager = _page->getSearchManager();

		onReloadPage(_page);

		// Create connections between Page signals and Scene slots
		{
			connect(_page, SIGNAL(signalDeleteAllItems(Page*)),
					this, SLOT(onDeleteAllItems(Page*)));

			connect(_page, SIGNAL(signalReloadPage(Page*)),
					this, SLOT(onReloadPage(Page*)));

			connect(_page, SIGNAL(signalItemRemove(QUuid)),
					this, SLOT(onItemRemove(QUuid)));

			connect(_page, SIGNAL(signalItemUpdate(QUuid)),
					this, SLOT(onItemUpdate(QUuid)));

			connect(_page, SIGNAL(signalItemRedraw(QUuid)),
					this, SLOT(onItemRedraw(QUuid)));

			connect(_page, SIGNAL(signalItemDisplayUpdate(QUuid)),
					this, SLOT(onItemDisplayUpdate(QUuid)));

			connect(_page, SIGNAL(signalItemTransformUpdate(QUuid)),
					this, SLOT(onItemTransformUpdate(QUuid)));

			connect(_page, SIGNAL(signalItemIdUpdate(QUuid,QUuid)),
					this, SLOT(onItemIdUpdate(QUuid,QUuid)));

			connect(_page, SIGNAL(signalItemAdd(AbstractModelItem*)),
					this, SLOT(onItemAdd(AbstractModelItem*)));

			connect(_page, SIGNAL(signalComponentAdd(Component*)),
					this, SLOT(onComponentAdd(Component*)));

			connect(_page, SIGNAL(signalAssemblyAdd(Assembly*)),
					this, SLOT(onAssemblyAdd(Assembly*)));

			connect(_page, SIGNAL(signalAssemblyMerge(Assembly*,Assembly*)),
					this, SLOT(onAssemblyMerge(Assembly*,Assembly*)));

			connect(_page, SIGNAL(signalComponentMerge(Component*,Component*)),
					this, SLOT(onComponentMerge(Component*,Component*)));

			connect(_page, SIGNAL(signalAssemblyDelete(Assembly*)),
					this, SLOT(onAssemblyDelete(Assembly*)));

			connect(_page, SIGNAL(signalComponentDelete(Component*)),
					this, SLOT(onComponentDelete(Component*)));
		}
	}

	SketchScene::~SketchScene()
	{
		QLOG_INFO() << "Deleting SketchScene";
		clear();
	}

	void SketchScene::clear()
	{
		QHash<QUuid, QGraphicsItem*>::const_iterator iter;
		for(iter = item_key_hash.constBegin();
			iter != item_key_hash.constEnd();
			++iter)
		{
			QGraphicsItem* graphicsitem = iter.value();
			delete graphicsitem;
		}
		item_key_hash.clear();
		searchResults.clear();
		freeStrokes.clear();
	}

	QImage SketchScene::getSelectionImage(QPolygonF selectionPolygon)
	{
		// Selection polygon should consist of at least 2 vertices
		if (selectionPolygon.size() < 3) return QImage();

		clearSelection();

		// Get the AABB of selection
		QRectF boundingBox = selectionPolygon.boundingRect();
		setSceneRect(itemsBoundingRect());

		// Create emmpty image extending from origin of scene to the edge of AABB
		QImage image(boundingBox.x() + boundingBox.width(),
					 boundingBox.y() + boundingBox.height(),
					 QImage::Format_ARGB32_Premultiplied);
		image.fill(Qt::transparent);

		// Create region mask based on the selection polygon
		QRegion region = QRegion(selectionPolygon.toPolygon());

		// Set up painter as well as clipping region
		QPainter painter(&image);
		painter.setClipping(true);
		painter.setClipRegion(region);
		painter.setRenderHint(QPainter::Antialiasing, true);

		// NOTE - Rendering this creates a top left area filled with black
		// Therefore render each graphics item individually instead
		//render(&painter);	// render scene items into painter
		for (int i=0; i < freeStrokes.size(); i++)
		{
			painter.setTransform(freeStrokes[i]->sceneTransform());
			freeStrokes[i]->paint(&painter, NULL);
		}

		// Make sure the image is square
		QRect intRect = boundingBox.toRect();
		int maxDim = (intRect.width() > intRect.height() ?
						  intRect.width() : intRect.height());
		QPoint center = intRect.center();

		// NOTE that this might fail if your selection area is in top-left
		// making the rectangle to lie outside 1st quadrant
		QRect squareRect = QRect(center.x()-maxDim/2, center.y()-maxDim/2,
								 maxDim, maxDim);

		// Crop the image based on selection
		QImage croppedSelection = image.copy(squareRect);
		return croppedSelection;
	}

	QImage SketchScene::getSelectionImageFromHighlight()
	{
		QRectF sceneRect = itemsBoundingRect();
		int x_min = sceneRect.x() + sceneRect.width();
		int x_max = sceneRect.x();
		int y_min = sceneRect.y() + sceneRect.height();
		int y_max = sceneRect.y();

		QImage image(sceneRect.x()+sceneRect.width(),
					 sceneRect.y()+sceneRect.height(),
					 QImage::Format_ARGB32_Premultiplied);
		image.fill(Qt::transparent);

		QPainter painter(&image);
		painter.setRenderHint(QPainter::Antialiasing, true);
		QHash<QUuid, QGraphicsItem*>::const_iterator iter;
		for(iter = item_key_hash.constBegin();
			iter != item_key_hash.constEnd();
			++iter)
		{
			QGraphicsItem* item = iter.value();
			if (item->type() == GraphicsPathItem::Type)
			{
				GraphicsPathItem* stroke =
						qgraphicsitem_cast<GraphicsPathItem*>(item);
				if (stroke->isHighlighted())
				{
					painter.setTransform(stroke->sceneTransform());
					stroke->highlight(false);
					stroke->paint(&painter, NULL);
					stroke->highlight(true);

					QRect rect=
							item->sceneTransform().inverted().mapRect(item->boundingRect()).toRect();
					x_min = (x_min < rect.x() ? x_min : rect.x());
					y_min = (y_min < rect.y() ? y_min : rect.y());

					x_max = (rect.x()+rect.width() > x_max) ? rect.x()+rect.width() : x_max;
					y_max = (rect.y()+rect.height() > y_max) ? rect.y()+rect.height() : y_max;
				}
			}
		}

		int maxDim = ( (x_max-x_min) > (y_max-y_min) ? (x_max-x_min) : (y_max-y_min) );
		QRect squareRect = QRect(x_min, y_min, maxDim, maxDim);
		QImage croppedSelection = image.copy(squareRect);
		return croppedSelection;
	}

	void SketchScene::drawBackground(QPainter *painter, const QRectF &rect)
	{
		QGraphicsScene::drawBackground(painter, rect);
	}

	GraphicsItemGroup *SketchScene::addComponent()
	{
		if (_page== NULL) return NULL;

		Component* c = _page->createComponent();
		GraphicsItemGroup* group = new GraphicsItemGroup();
		addItem(group);

		group->component = c;
		item_key_hash.insert(c->id(), group);
		return group;
	}

	GraphicsPathItem* SketchScene::addStroke(GraphicsItemGroup *parentItem,
											 QColor color, float thickness)
	{
		if (parentItem == NULL || _page == NULL) return NULL;
		Stroke* s = parentItem->component->addStroke(color, thickness);
		GraphicsPathItem* item = new GraphicsPathItem(parentItem, s);
		parentItem->addToGroup(item);
		item_key_hash.insert(s->id(), item);
		freeStrokes.push_back(item);
		return item;
	}

	GraphicsPolygon2D* SketchScene::addPolygon(GraphicsItemGroup *parentItem)
	{
		if (parentItem == NULL || _page == NULL) return NULL;
		Polygon2D* p = new Polygon2D(parentItem->component);
		parentItem->component->addItem(p);

		GraphicsPolygon2D* item = new GraphicsPolygon2D(NULL, p);
		parentItem->addToGroup(item);
		return item;
	}

	void SketchScene::insertItem(GraphicsItemGroup *child, GraphicsItemGroup *parent)
	{

	}

	void SketchScene::insertItem(GraphicsPathItem *child, GraphicsItemGroup *parent)
	{
		if (parent->component->containsItem(child->parentStroke()->id()) == false)
		{

		}
		parent->addToGroup(child);
	}

	void SketchScene::insertItem(GraphicsPolygon2D *child, GraphicsItemGroup *parent)
	{
		if (parent->component->containsItem(child->parentPolygon->id()))
		{
			parent->addToGroup(child);
		}
	}

	QImage SketchScene::getSelectionImage()
	{
		if (freeStrokes.size() == 0 ) return QImage();
		QRectF rect = sceneRect();
		int x_min = 0;  int y_min = 0;
		int x_max = rect.x() + rect.width();
		int y_max = rect.y() + rect.height();
		QImage image(x_max,y_max, QImage::Format_ARGB32);
		image.fill(Qt::transparent);

		clearSelection();
		setSceneRect(itemsBoundingRect());

		GraphicsPathItem* item = freeStrokes[freeStrokes.size()-1];
		QPainter painter(&image);
		painter.setRenderHint(QPainter::Antialiasing);
		x_min = x_max; y_min = y_max; x_max = 0; y_max = 0;
		for (int i=0; i<freeStrokes.size(); i++)
		{
			item = freeStrokes[i];

			painter.setTransform(item->sceneTransform());

			item->paint(&painter, NULL);

			QLOG_INFO() << "ERROR! SketchScene::getSelectionImage() does not transform to scene coords";
			QRect rect = item->boundingRect().toRect();
			x_min = (x_min < rect.x() ? x_min : rect.x());
			y_min = (y_min < rect.y() ? y_min : rect.y());

			x_max = (rect.x()+rect.width() > x_max) ? rect.x()+rect.width() : x_max;
			y_max = (rect.y()+rect.height() > y_max) ? rect.y()+rect.height() : y_max;
		}
		QImage croppedSelection = image.copy(QRect(x_min,y_min,
												   x_max-x_min, y_max-y_min));
		croppedSelection.save("dummy.png");
		return croppedSelection;
	}

	QList<GraphicsPathItem*> SketchScene::getSelectedStrokes(Point2D pos, float margin)
	{
		QList<GraphicsPathItem*> selectedStrokes = QList<GraphicsPathItem*>();
		QHash<QUuid, QGraphicsItem*>::const_iterator iter;
		for(iter = item_key_hash.constBegin();
			iter != item_key_hash.constEnd();
			++iter)
		{
			if (iter.value()->isVisible() == false) continue;
			if (iter.value()->type() == GraphicsPathItem::Type)
			{
				GraphicsPathItem* strokeItem =
						qgraphicsitem_cast<GraphicsPathItem*>(iter.value());
				Point2D mappedPos = strokeItem->sceneTransform().inverted().map(pos);
				if (strokeItem->parentStroke()== NULL) continue;
				if (strokeItem->parentStroke()->containsPoint(mappedPos, OnItem, margin))
					selectedStrokes.push_back(strokeItem);
				// Do we scale the margin based on transform?
			}
		}
		return selectedStrokes;
	}

	QList<GraphicsPathItem*> SketchScene::getSelectedStrokes
	(QPolygonF selectionPolygon, float minimalAllowedSelection)
	{
		QList<GraphicsPathItem*> selectedStrokes;
		QHash<QUuid, QGraphicsItem*>::const_iterator iter;
		for (iter = item_key_hash.constBegin();
			 iter != item_key_hash.constEnd();
			 ++iter)
		{
			QGraphicsItem* item = iter.value();
			if (item->type() != GraphicsPathItem::Type) continue;
			GraphicsPathItem* strokeItem =
					qgraphicsitem_cast<GraphicsPathItem*>(item);
			if (strokeItem->parentStroke()== NULL) continue;
			// 1. Map the polygon to the stroke's coordinate system
			QPolygonF mappedPolygon =
					strokeItem->sceneTransform().inverted().map(selectionPolygon);
			if (strokeItem->parentStroke()->isContainedWithin(&mappedPolygon, minimalAllowedSelection))
			{
				selectedStrokes.push_back(strokeItem);
			}
		}
		return selectedStrokes;
	}

	QList<GraphicsPathItem*> SketchScene::getHighlightedStrokes()
	{
		QList<GraphicsPathItem*> selectedStrokes;
		QHash<QUuid, QGraphicsItem*>::const_iterator iter;
		for (iter = item_key_hash.constBegin();
			 iter != item_key_hash.constEnd();
			 ++iter)
		{
			QGraphicsItem* graphicsitem = iter.value();
			if (graphicsitem->type() == GraphicsPathItem::Type)
			{
				GraphicsPathItem* pathitem = qgraphicsitem_cast<GraphicsPathItem*>
						(graphicsitem);
				if (pathitem->isHighlighted())
					selectedStrokes.push_back(pathitem);
			}
		}
		return selectedStrokes;
	}

	void SketchScene::SelectSearchResult(SearchGraphicsItem *searchItem)
	{
		// Obsolete
	}

	void SketchScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
	{
		mouseEvent->accept();
		emit signalMouseEvent(mouseEvent, 0);
	}

	void SketchScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
	{
		mouseEvent->accept();
		emit signalMouseEvent(mouseEvent, 1);
	}

	void SketchScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
	{
		mouseEvent->accept();
		emit signalMouseEvent(mouseEvent, 2);
	}

	void SketchScene::OnSearchComplete()
	{
		//		// Read file from SearchManager
		//		for (int i=0; i< searchResults.size(); i++)
		//		{
		//			SearchGraphicsItem* item = searchResults[i];
		//			removeItem(item);
		//			delete (item);
		//		}
		//		searchResults.clear();
		//		int searchItemSize = 150;
		//		int margin = 10;
		//		int offset = 0;
		//		int maxIndex = searchManager->localFileList.size();

		//		// Iterators work fine as well. Just not needed.
		//		for (int index=0;index < maxIndex; index++)
		//		{
		//			QString imagePath = searchManager->localFileList[index];
		//			qDebug() << "Loading search result image at " << imagePath;
		//			QPixmap pix(imagePath);
		//			if (pix.height() > searchItemSize)
		//				pix = pix.scaledToHeight(searchItemSize, Qt::SmoothTransformation);
		//			if (pix.width() > searchItemSize)
		//				pix = pix.scaledToWidth(searchItemSize, Qt::SmoothTransformation);
		//			SearchGraphicsItem* searchItem = new SearchGraphicsItem(pix,imagePath, NULL);
		//			addItem(searchItem);
		//			searchItem->setOffset(offset, 0);
		//			offset += searchItemSize + margin;
		//			searchResults.append(searchItem);
		//		}
	}

	void SketchScene::clearHighlight()
	{
		QHash<QUuid, QGraphicsItem*>::const_iterator iter;
		for(iter = item_key_hash.constBegin();
			iter != item_key_hash.constEnd();
			++iter)
		{
			QGraphicsItem* item = iter.value();
			if (item->type() == GraphicsPathItem::Type)
			{
				GraphicsPathItem* graphicspathitem = qgraphicsitem_cast<GraphicsPathItem*>(item);
				if (graphicspathitem->isHighlighted())
					graphicspathitem->highlight(false);
			}
		}
	}

	//	void SketchScene::onItemDisplayStatusChanged(AbstractModelItem* item)
	//	{
	////		if (item_key_hash.contains(item->id()))
	////		{
	////			QGraphicsItem* graphicsItem = item_key_hash.value(item->id());
	////			if (item->isVisible())
	////				graphicsItem->show();
	////			else
	////				graphicsItem->hide();
	////		}
	//	}

	//    void SketchScene::onItemDestroy(AbstractModelItem* item)
	//	{
	////		qDebug() << item->id().toString() << " is Item ID";
	////		if (item_key_hash.contains(item->id()))
	////		{
	////			qDebug() << "Entered loop";
	////			QGraphicsItem* graphicsItem = item_key_hash.value(item->id());
	////			// Avoid duplicate calls
	////			qDebug() << item_key_hash.remove(item->id()) << " items removed";
	////			if (item->type() == STROKE)
	////			{
	////				GraphicsPathItem* graphicspath = static_cast<GraphicsPathItem*>(graphicsItem);
	////				if (freeStrokes.contains(graphicspath))
	////					freeStrokes.removeAll(graphicspath);
	////			}
	////			delete graphicsItem;
	////		}
	//	}

	//	void SketchScene::onItemTransformChanged(AbstractModelItem* item)
	//	{
	////        if (item_key_hash.contains(item->id()) == false) return;

	////        QGraphicsItem* graphicsItem = item_key_hash.value(item->id());
	////        graphicsItem->setTransform(item->transform());
	////        update(graphicsItem->mapRectToScene(graphicsItem->boundingRect()));
	//	}

	//	void SketchScene::onItemParentChange(AbstractModelItem* item)
	//	{
	////		if (item_key_hash.contains(item->id()) == false) return;
	////		QGraphicsItem* graphicsItem = item_key_hash.value(item->id());

	////		AbstractModelItem* parentItem = item->parentItem();
	////		if (parentItem != NULL) return;

	////		if ((item_key_hash.contains(parentItem->id())
	////			 && parentItem->type() == COMPONENT) == false) return;
	////		GraphicsItemGroup* parentGroup =
	////				static_cast<GraphicsItemGroup*>(item_key_hash.value(parentItem->id()));
	////		parentGroup->addToGroup(graphicsItem);
	////		// TODO - DO not update the transformation here. Rather use the item transform()

	////		// NOTE - Add extra AbstractModelItem objects as and when they are being supported
	////		if (item->type() == STROKE)
	////		{
	////			Stroke* stroke = static_cast<Stroke*>(item);
	////			stroke->setTransform(graphicsItem->transform());
	////		} else if (item->type() == POLYGON2D)
	////		{
	////			Polygon2D* polygon = static_cast<Polygon2D*>(item);
	////			polygon->setTransform(graphicsItem->transform());
	////		} else if (item->type() == IMAGE)
	////		{
	////			Image* image = static_cast<Image*>(item);
	////			image->setTransform(graphicsItem->transform());
	////		}
	//	}
	//
	//////////////////////////////////////////////////////////////////
	void SketchScene::onDeleteAllItems(Page* page)
	{
		clear();
	}

	void SketchScene::onReloadPage(Page* page)
	{
		if (_page == NULL) return;
		if (_page != page) return;

		clear();

		// Loading page
		QList<Component*> c = _page->getComponents();
		foreach (Component* comp, c)
		{
			onComponentAdd(comp);
			QList<AbstractModelItem*> modelitems = comp->values();
			foreach (AbstractModelItem* item, modelitems)
			{
				onItemAdd(item);
			}
		}
	}

	void SketchScene::onItemRemove(QUuid itemId)
	{
		if (item_key_hash.contains(itemId))
		{
			QGraphicsItem* item = item_key_hash.value(itemId);
			item_key_hash.remove(itemId);
			delete item;
		}
	}

	void SketchScene::onItemUpdate(QUuid itemId)
	{
		if (item_key_hash.contains(itemId) == false) return;
		QGraphicsItem* item = item_key_hash.value(itemId);
		switch (item->type())
		{
		case GraphicsPathItem::Type :
		{
			GraphicsPathItem* stroke = qgraphicsitem_cast<GraphicsPathItem*>(item);
			stroke->updateStroke();
			break;
		}
		case GraphicsPolygon2D::Type :
		{
			GraphicsPolygon2D* polygon = qgraphicsitem_cast<GraphicsPolygon2D*>(item);
			polygon->updatePolygon();
			break;
		}
		case GraphicsPixmap::Type :
		{
			GraphicsPixmap* pixmap = qgraphicsitem_cast<GraphicsPixmap*>(item);
			pixmap->updateImage();
			break;
		}
		case GraphicsItemGroup::Type :
			GraphicsItemGroup* itemgroup = qgraphicsitem_cast<GraphicsItemGroup*>(item);
			itemgroup->updateGroup();
		}
	}

	void SketchScene::onItemRedraw(QUuid itemId)
	{
		if (item_key_hash.contains(itemId))
		{
			QGraphicsItem* item = item_key_hash.value(itemId);
			update(item->mapRectToScene(item->boundingRect()));
		}
	}

	void SketchScene::onItemDisplayUpdate(QUuid itemId)
	{
		if (item_key_hash.contains(itemId) == false) return;
		QGraphicsItem* item = item_key_hash.value(itemId);
		bool dispStatus = item->isVisible();
		if (item->type() == GraphicsPathItem::Type)
		{
			GraphicsPathItem* stroke = qgraphicsitem_cast<GraphicsPathItem*>(item);
			dispStatus = stroke->parentStroke()->isVisible();
		} else if (item->type() == GraphicsPolygon2D::Type)
		{
			GraphicsPolygon2D* polygon = qgraphicsitem_cast<GraphicsPolygon2D*>(item);
			dispStatus = polygon->parentPolygon->isVisible();
		} else if(item->type() == GraphicsPixmap::Type)
		{
			GraphicsPixmap* pixmap = qgraphicsitem_cast<GraphicsPixmap*>(item);
			dispStatus = pixmap->parentImage()->isVisible();
		} else if(item->type() == GraphicsItemGroup::Type)
		{
			// NOTE - There is no display option for components yet.
			// Fill this section when it is available
		}

		// Check if display status is different from given current status
		if (dispStatus != item->isVisible())
		{
			if (dispStatus)
				item->show();
			else
				item->hide();
		}
	}

	void SketchScene::onItemTransformUpdate(QUuid itemId)
	{
		if (item_key_hash.contains(itemId) == false) return;
		QGraphicsItem* item = item_key_hash.value(itemId);
		if (item->type() == GraphicsPathItem::Type)
		{
			GraphicsPathItem* stroke = qgraphicsitem_cast<GraphicsPathItem*>(item);
			stroke->setTransform(stroke->parentStroke()->transform());
		} else if (item->type() == GraphicsPolygon2D::Type)
		{
			GraphicsPolygon2D* polygon = qgraphicsitem_cast<GraphicsPolygon2D*>(item);
			polygon->setTransform(polygon->parentPolygon->transform());
		} else if(item->type() == GraphicsPixmap::Type)
		{
			GraphicsPixmap* pixmap = qgraphicsitem_cast<GraphicsPixmap*>(item);
			pixmap->setTransform(pixmap->parentImage()->transform());
		} else if(item->type() == GraphicsItemGroup::Type)
		{
			GraphicsItemGroup* itemgroup = qgraphicsitem_cast<GraphicsItemGroup*>(item);
			itemgroup->setTransform(itemgroup->component->transform());
		}
	}

	void SketchScene::onItemIdUpdate(QUuid oldID, QUuid newID)
	{
		if (item_key_hash.contains(oldID))
		{
			QGraphicsItem* item = item_key_hash.value(oldID);
			item_key_hash.remove(oldID);
			item_key_hash.insert(newID, item);
		}
	}

	void SketchScene::onItemAdd(AbstractModelItem* item)
	{
		Component* c = item->parentItem();
		GraphicsItemGroup* group = NULL;
		if (item_key_hash.contains(c->id()))
		{
			group = qgraphicsitem_cast<GraphicsItemGroup*>(item_key_hash.value(c->id()));
		} else {
			group = addComponent();
			group->component->addItem(item);
		}
		QGraphicsItem* newGraphicsItem = NULL;
		if (item->type()== STROKE)
		{
			newGraphicsItem = new GraphicsPathItem(group, static_cast<Stroke*>(item));
		}
		if (item->type() == POLYGON2D)
		{
			newGraphicsItem = new GraphicsPolygon2D(group, static_cast<Polygon2D*>(item));
		}
		if (item->type() == IMAGE)
		{
			QLOG_INFO() << "GraphicsPixmapItem not defined";
		}

		if (newGraphicsItem != NULL)
		{
			item_key_hash.insert(item->id(), newGraphicsItem);
			group->addToGroup(newGraphicsItem);
			newGraphicsItem->setTransform(item->transform());
		}
	}

	// Called only from within class
	void SketchScene::onComponentAdd(Component* component)
	{
		if (item_key_hash.contains(component->id()) == false)
		{
			GraphicsItemGroup* group = new GraphicsItemGroup();
			addItem(group);
			group->component = component;
			item_key_hash.insert(component->id(), group);
			group->updateGroup();
		}
	}

	void SketchScene::onAssemblyAdd(Assembly* assembly)
	{

	}

	void SketchScene::onComponentMerge(Component* a, Component* b)
	{

	}

	void SketchScene::onAssemblyMerge(Assembly* a, Assembly* b)
	{

	}

	void SketchScene::onAssemblyUpdate(Assembly* assembly)
	{

	}

	void SketchScene::onComponentUpdate(Component* component)
	{
		if (item_key_hash.contains(component->id()))
		{
			QGraphicsItem* item = item_key_hash.value(component->id());
			if (item->type() == GraphicsItemGroup::Type)
			{
				GraphicsItemGroup* group = qgraphicsitem_cast<GraphicsItemGroup*>(item);
				group->updateGroup();
				update();
			}
		}
	}

	// Equivalent to deleting object
	void SketchScene::onAssemblyDelete(Assembly* assembly)
	{
		if (item_key_hash.contains(assembly->id()))
		{
			GraphicsItemGroup* group = qgraphicsitem_cast<GraphicsItemGroup*>(item_key_hash.value(assembly->id()));
			item_key_hash.remove(assembly->id());
			delete group;
		}
	}

	void SketchScene::onComponentDelete(Component* component)
	{
		if (item_key_hash.contains(component->id()))
		{
			GraphicsItemGroup* group =
					qgraphicsitem_cast<GraphicsItemGroup*>(item_key_hash.value(component->id()));
			item_key_hash.remove(component->id());
			delete group;
		}
	}

}
