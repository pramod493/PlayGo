#include "sketchscene.h"
#include <QtAlgorithms>
#include <QList>
#include "QsLog.h"
#include "component.h"
#include "stroke.h"
#include "polygon2d.h"
#include "pixmap.h"
#include <QGraphicsWebView>
#include <QUrl>

#define CDI_USE_MOUSE_TO_MOVE

namespace CDI
{
	SketchScene::SketchScene(Page *page, QObject* parent) :
		QGraphicsScene(parent)
	{
		_page = page;
		// Do not set up any connections when _page is null

		if (_page != NULL) _page->setScene(this);

	}

	SketchScene::~SketchScene()
	{
		// Since sketch scene does not create these items,
		// we should simply remove them from scene
		clear();
	}

	void SketchScene::clear()
	{
		QLOG_INFO() << "Removing all items from the scene";
		QList<QGraphicsItem*> allitems = items();
		for (int i=0; i < allitems.size(); i++)
		{
			QGraphicsItem* graphicsitem = allitems[i];
			removeItem(graphicsitem);
		}
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
		QList<QGraphicsItem*> allitems = items();
		for (int i=0; i < allitems.size(); i++)
		{
			QGraphicsItem* graphicsitem = allitems[i];
			// TODO Check if the selection polygon and AABB intersect
			if (graphicsitem->type() == Stroke::Type && graphicsitem->isVisible())
			{
				painter.setTransform(graphicsitem->sceneTransform());
				graphicsitem->paint(&painter, NULL);
			}
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
		QList<QGraphicsItem*> allitems = items();
		for (int i=0; i < allitems.size(); i++)
		{
			QGraphicsItem* graphicsitem = allitems[i];
			// TODO Check if the selection polygon and AABB intersect
			if (graphicsitem->type() == Stroke::Type && graphicsitem->isVisible())
			{
				Stroke* stroke = qgraphicsitem_cast<Stroke*>(graphicsitem);
				if (stroke->isHighlighted())
				{
					stroke->highlight(false);
					painter.setTransform(stroke->sceneTransform());
					stroke->paint(&painter, NULL);
					stroke->highlight(true);

					QRect rect=
							stroke->sceneTransform().inverted().mapRect(stroke->boundingRect()).toRect();

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

	QImage SketchScene::getSelectionImage()
	{
		//		if (freeStrokes.size() == 0 ) return QImage();
		QRectF rect = sceneRect();
		int x_min = 0;  int y_min = 0;
		int x_max = rect.x() + rect.width();
		int y_max = rect.y() + rect.height();
		QImage image(x_max,y_max, QImage::Format_ARGB32);
		image.fill(Qt::transparent);

		QPainter painter(&image);
		painter.setRenderHint(QPainter::Antialiasing);
		x_min = x_max; y_min = y_max; x_max = 0; y_max = 0;
		QList<QGraphicsItem*> allitems = items();
		for (int i=0; i<allitems.size(); i++)
		{
			QGraphicsItem* graphicsitem = allitems[i];

			if (graphicsitem->isVisible() == false) continue;
			if (graphicsitem->type() != Stroke::Type) continue;

			painter.setTransform(graphicsitem->sceneTransform());
			graphicsitem->paint(&painter, NULL);

			QRect rect =
					graphicsitem->sceneTransform().inverted().mapRect(
						graphicsitem->boundingRect().toRect());

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

	QList<Stroke*> SketchScene::getSelectedStrokes(Point2D pos, float margin)
	{
		QList<Stroke*> selectedStrokes = QList<Stroke*>();
		QList<QGraphicsItem*> allitems = items();

		for(QList<QGraphicsItem*>::const_iterator iter = allitems.constBegin();
			iter != allitems.constEnd(); ++iter)
		{
			QGraphicsItem* graphicsitem = (*iter);
			if (graphicsitem->isVisible() == false) continue;
			if (graphicsitem->type() != Stroke::Type) continue;
			Stroke* stroke =
					qgraphicsitem_cast<Stroke*>(graphicsitem);
			Point2D mappedPos = stroke->sceneTransform().inverted().map(pos);
			if (stroke->contains(mappedPos, margin))
				selectedStrokes.push_back(stroke);
		}
		return selectedStrokes;
	}

	QList<Stroke*> SketchScene::getSelectedStrokes
	(QPolygonF selectionPolygon, float minimalAllowedSelection)
	{
		QList<Stroke*> selectedStrokes;
		QList<QGraphicsItem*> allitems = items();
		for(QList<QGraphicsItem*>::const_iterator iter = allitems.constBegin();
			iter != allitems.constEnd(); ++iter)
		{
			QGraphicsItem* graphicsitem = (*iter);
			if (graphicsitem->isVisible() == false) continue;
			if (graphicsitem->type() != Stroke::Type) continue;
			Stroke* stroke=
					qgraphicsitem_cast<Stroke*>(graphicsitem);
			QPolygonF mappedPolygon =
					stroke->sceneTransform().inverted().map(selectionPolygon);
			if (stroke->isContainedWithin(&mappedPolygon, minimalAllowedSelection))
				selectedStrokes.push_back(stroke);
		}
		return selectedStrokes;
	}

	QList<Stroke*> SketchScene::getHighlightedStrokes()
	{
		QList<Stroke*> selectedStrokes;
		QList<QGraphicsItem*> allitems = items();
		for(QList<QGraphicsItem*>::const_iterator iter = allitems.constBegin();
			iter != allitems.constEnd(); ++iter)
		{
			QGraphicsItem* graphicsitem = (*iter);
			if (graphicsitem->isVisible() == false) continue;
			if (graphicsitem->type() != Stroke::Type) continue;
			Stroke* stroke =
					qgraphicsitem_cast<Stroke*>(graphicsitem);
			if (stroke->isHighlighted())
				selectedStrokes.push_back(stroke);
		}
		return selectedStrokes;
	}

	QList<QGraphicsItem *> SketchScene::getSelectedItems(Point2D pos, float margin)
	{
		QList<QGraphicsItem*>  selectedItems;
		QList<QGraphicsItem*> allitems = items(pos, Qt::IntersectsItemBoundingRect,
											   Qt::AscendingOrder, QTransform());
		foreach (QGraphicsItem* graphicsitem, allitems)
		{
			bool selectParentItem = false;
			switch (graphicsitem->type())
			{
			case Pixmap::Type :
			case Polygon2D::Type :
			{
				if (graphicsitem->contains(graphicsitem->mapFromScene(pos)) &&
						graphicsitem->isVisible())
				{
					if (graphicsitem->parentItem())
					{
						selectParentItem = true;
					} else
					{
						if ( selectedItems.contains(graphicsitem) == false)
							 selectedItems.push_back(graphicsitem);
					}
				}
				break;
			}
			case Stroke::Type :
			{
				Stroke* stroke = qgraphicsitem_cast<Stroke*>(graphicsitem);
				if (stroke->isVisible() && stroke->contains(stroke->mapFromScene(pos), margin))
				{
//					if (graphicsitem->parentItem())
//					{
//						selectParentItem = true;
//					} else
//					{
						if ( selectedItems.contains(graphicsitem) == false)
							 selectedItems.push_back(graphicsitem);
//					}
				}
				break;
			}
			}
			if (selectParentItem)
			{
				QGraphicsItem* parent = graphicsitem->parentItem();
				if (parent->type() == Component::Type &&
						( selectedItems.contains(parent) == false))
				{
					 selectedItems.push_back(parent);
				}
			}
		}
		return  selectedItems;
	}

	QRectF SketchScene::getBoundingBox(QList<QGraphicsItem *> listOfItems)
	{
		if (listOfItems.size() == 0)
		{
			QLOG_ERROR() << "List of selected items is empty.";
			return QRectF();
		}

		QRectF baseRect = listOfItems[0]->sceneTransform().inverted().mapRect(listOfItems[0]->boundingRect());
		for (int i=1; i < listOfItems.size(); i++)
		{
			baseRect = baseRect.united(
						listOfItems[i]->sceneTransform().inverted().mapRect
						(listOfItems[i]->boundingRect()));
		}
		return baseRect;
	}

	void SketchScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
	{
		mouseEvent->accept();
		if (mouseEvent->button() != Qt::NoButton)
		{
			emit signalMouseEvent(mouseEvent, 0);
		}
#ifdef CDI_USE_MOUSE_TO_MOVE
		QGraphicsScene::mousePressEvent(mouseEvent);
#endif //CDI_USE_MOUSE_TO_MOVE
	}

	void SketchScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
	{
		mouseEvent->accept();
		if (mouseEvent->button() != Qt::NoButton)
		{
			emit signalMouseEvent(mouseEvent, 1);
		}
#ifdef CDI_USE_MOUSE_TO_MOVE
		QGraphicsScene::mouseMoveEvent(mouseEvent);
#endif //CDI_USE_MOUSE_TO_MOVE
	}

	void SketchScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
	{
		mouseEvent->accept();
		if (mouseEvent->button() != Qt::NoButton)
		{
			emit signalMouseEvent(mouseEvent, 2);
		}
#ifdef CDI_USE_MOUSE_TO_MOVE
		QGraphicsScene::mouseReleaseEvent(mouseEvent);
#endif //CDI_USE_MOUSE_TO_MOVE
	}

	void SketchScene::clearStrokeHighlight()
	{
		QList<QGraphicsItem*> allitems = items();
		for(QList<QGraphicsItem*>::const_iterator iter = allitems.constBegin();
			iter != allitems.constEnd(); ++iter)
		{
			QGraphicsItem* graphicsitem = (*iter);
			if (graphicsitem->type() != Stroke::Type) continue;
			Stroke* stroke = qgraphicsitem_cast<Stroke*>(graphicsitem);
			if (stroke->isHighlighted()) stroke->highlight(false);
		}
	}
}
