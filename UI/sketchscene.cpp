#include "sketchscene.h"
#include <QtAlgorithms>
#include <QDebug>
#include <QList>

#include "graphicsitemgroup.h"
#include "graphicspathitem.h"
#include "graphicspolygon2d.h"

namespace CDI
{
	SketchScene::SketchScene(Page *page, QObject* parent) :
		QGraphicsScene(parent)
	{
		_page = page;
		searchResults = QList<SearchGraphicsItem*>();
		freeStrokes = QList<GraphicsPathItem*>();
		// searchManager = new SearchManager(this);
		searchManager = SearchManager::instance();
	}

	SketchScene::~SketchScene()
	{
		qDebug() << "Deleting SketchScene";
		clear();
	}

	void SketchScene::clear()
	{
		// Delete all items and clear the scene
		size_t n = searchResults.size();
		for (int i = 0; i < n; i++)
		{
			SearchGraphicsItem* item = searchResults[i];
			removeItem(item);
			delete item;
		}
		searchResults.clear();

		n = freeStrokes.size();
		for (int i=0; i<n; i++)
		{
			GraphicsPathItem* item = freeStrokes[i];
			removeItem(item);
			delete item;
		}
		freeStrokes.clear();

		// Search for leftover items in scene
		// This one throws error
		/*
		QList<QGraphicsItem*> L = items();
		while (!L.empty())
		{
			if (L.first()== NULL) continue;
			removeItem(L.first());
			delete L.first();
			L.removeFirst();
		 }
		 */
		// Use this instead
		qDeleteAll(items());

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
			freeStrokes[i]->paint(&painter, NULL);

		// Crop the image based on selection
		QImage croppedSelection = image.copy(boundingBox.toRect());
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
		group->component = c;
		item_key_hash.insert(c->id(), group);
		return group;
	}

	GraphicsPathItem* SketchScene::addStroke(GraphicsItemGroup *parentItem,
											 QColor color, float thickness)
	{
		if (parentItem == NULL || _page == NULL) return NULL;
		Stroke* s = parentItem->component->addStroke(color, thickness);
		GraphicsPathItem* item = new GraphicsPathItem(NULL, s);
		item_key_hash.insert(s->id(), item);
		parentItem->addToGroup(item);
		return item;
	}

	GraphicsPolygon2D* SketchScene::addPolygon(GraphicsItemGroup *parentItem)
	{
		if (parentItem == NULL || _page == NULL) return NULL;
		Polygon2D* p = new Polygon2D();
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
		if (parent->component->containsItem(child->parentStroke->id()) = false)
		{

		}
		parent->addToGroup(child);
	}

	void SketchScene::insertItem(GraphicsPolygon2D *child, GraphicsItemGroup *parent)
	{

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
			item->paint(&painter, NULL);
			QRect rect = item->boundingRect().toRect();
			x_min = (x_min < rect.x() ? x_min : rect.x());
			y_min = (y_min < rect.y() ? y_min : rect.y());

			x_max = (rect.x()+rect.width() > x_max) ? rect.x()+rect.width() : x_max;
			y_max = (rect.y()+rect.height() > y_max) ? rect.y()+rect.height() : y_max;
		}
		QImage croppdSelection = image.copy(QRect(x_min,y_min,
												  x_max-x_min, y_max-y_min));
		return croppdSelection;
	}

	QList<GraphicsPathItem*> SketchScene::getSelectedStrokes(Point2D pos, float margin)
	{
		QList<GraphicsPathItem*> selectedStrokes = QList<GraphicsPathItem*>();
		for (int i=0;i<freeStrokes.size();i++)
			if (freeStrokes[i] != NULL)
				if (freeStrokes[i]->Selected(pos, margin))
					selectedStrokes.push_back(freeStrokes[i]);
		return selectedStrokes;
	}

	void SketchScene::SelectSearchResult(SearchGraphicsItem *searchItem)
	{
		Q_UNUSED(searchItem)
		// IMPORTANT - Implement loading of search results
		// TODO - Get the results file. Load the image and replace it
		// with all strokes with search results in a way that it fits
		// neatly in the same area
		//    QString imageSourceFile = searchItem->sourceFilePath;
		//    QPixmap pix = QPixmap(imageSourceFile); // Skip sanity checks since its coming from search image

		//    GraphicsItemGroup *itemGroup = new GraphicsItemGroup(NULL, NULL);
		//    addItem(itemGroup);
		//    // NOTES - In here we want to allow create all items as ItemGroup of give type.. would that be better?
		//    // Is it better to keep parent object = NULL if we are planning to reparent object during the workflow?
		//    PixmapItem* sceneItem = new PixmapItem(pix, imageSourceFile, NULL, NULL);
		//    itemGroup->addToGroup(sceneItem);
		//    // Add all the strokes but set the invisible
		//    for (QList<GraphicsPathItem*>::iterator it = freeStrokes.begin();
		//         it != freeStrokes.end();
		//         ++it)
		//    {
		//        itemGroup->addToGroup(*it);
		//        (*it)->setVisible(false);
		//    }
		//    freeStrokes.clear();
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
		// Read file from SearchManager
		for (int i=0; i< searchResults.size(); i++)
		{
			SearchGraphicsItem* item = searchResults[i];
			removeItem(item);
			delete (item);
		}
		searchResults.clear();
		int searchItemSize = 150;
		int margin = 10;
		int offset = 0;
		int maxIndex = searchManager->localFileList.size();

		// Iterators work fine as well. Just not needed.
		for (int index=0;index < maxIndex; index++)
		{
			QString imagePath = searchManager->localFileList[index];
			qDebug() << "Loading search result image at " << imagePath;
			QPixmap pix(imagePath);
			if (pix.height() > searchItemSize)
				pix = pix.scaledToHeight(searchItemSize, Qt::SmoothTransformation);
			if (pix.width() > searchItemSize)
				pix = pix.scaledToWidth(searchItemSize, Qt::SmoothTransformation);
			SearchGraphicsItem* searchItem = new SearchGraphicsItem(pix,imagePath, NULL);
			addItem(searchItem);
			searchItem->setOffset(offset, 0);
			offset += searchItemSize + margin;
			searchResults.append(searchItem);
		}
	}

	void SketchScene::onItemDisplayStatusChanged(AbstractModelItem* item)
	{
		if (item_key_hash.contains(item->id()))
		{
			QGraphicsItem* graphicsItem = item_key_hash.value(item->id());
			if (item->isVisible())
				graphicsItem->show();
			else
				graphicsItem->hide();
		}
	}

}
