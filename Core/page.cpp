#include "page.h"
#include "playgo.h"
#include <QtAlgorithms>
#include "QsLog.h"
#include "sketchscene.h"

namespace CDI
{
	// This cannot exist on its own. It must have a parent PlayGo. Though not sure
	// what will happen when its missing. we don't seem to use it often
	Page::Page(PlayGo* parent)
		: QObject(parent)
	{
		_id = uniqueHash();
		_playgo = parent;

		_assemblies = QHash<QUuid, Assembly*>();
		_components = QHash<QUuid, Component*>();

		_searchManager = new SearchManager(this);

		PhysicsSettings settings = PhysicsSettings();
		settings.gravity = Point2D(0,0);
		settings.timeStep = 1.0f/60.0f;
		_physicsManager = new PhysicsManager(settings, this);

		_scene = new SketchScene(this);

		_currentComponent = NULL;

		transformedComponents = QList<Component*>();

		connect(this, SIGNAL(signalItemAdd(QGraphicsItem*)),
				_physicsManager, SLOT(onItemAdd(QGraphicsItem*)));
		connect(_physicsManager, SIGNAL(physicsStepComplete()),
				this, SLOT(onSimulationStepComplete()));
		connect(_physicsManager, SIGNAL(physicsStepStart()),
				this, SLOT(onSimulationStepStart()));

	}

	Page::Page(const Page &page)
		:QObject(page.parent())
	{
		_id = uniqueHash();
		_playgo = NULL;

		_assemblies = QHash<QUuid, Assembly*>();
		_components = QHash<QUuid, Component*>();

		_searchManager = new SearchManager(this);

		PhysicsSettings settings = PhysicsSettings();
		settings.gravity = Point2D(0,0);
		settings.timeStep = 1.0f/60.0f;
		_physicsManager = new PhysicsManager(settings, this);

		_scene = new SketchScene(this);

		_currentComponent = NULL;

		transformedComponents = QList<Component*>();

		connect(this, SIGNAL(signalItemAdd(QGraphicsItem*)),
				_physicsManager, SLOT(onItemAdd(QGraphicsItem*)));
		connect(_physicsManager, SIGNAL(physicsStepComplete()),
				this, SLOT(onSimulationStepComplete()));
		connect(_physicsManager, SIGNAL(physicsStepStart()),
				this, SLOT(onSimulationStepStart()));
	}

	Page::~Page()
	{
		deleteAll();

		if (_searchManager) delete _searchManager;
		if (_physicsManager) delete _physicsManager;
	}

	QUuid Page::id() const
	{
		return _id;
	}

	SearchManager* Page::getSearchManager() const
	{
		return _searchManager;
	}

	PhysicsManager* Page::getPhysicsManager() const
	{
		return _physicsManager;
	}

	QList<Assembly*> Page::getAssemblies() const
	{
		return _assemblies.values();
	}

	QList<Component*> Page::getComponents() const
	{
		return _components.values();
	}

	QGraphicsScene* Page::scene() const
	{
		return _scene;
	}

	void Page::setScene(QGraphicsScene *scene)
	{
		_scene = scene;
		if (_scene == NULL) return;

		QHash<QUuid, Component*>::const_iterator componentiter;
		for (componentiter = _components.constBegin();
			 componentiter != _components.constEnd(); ++componentiter)
		{
			Component* component = componentiter.value();
			_scene->addItem(component);
		}
		QHash<QUuid, Assembly*>::const_iterator assemiter;
		for (assemiter = _assemblies.constBegin();
			 assemiter != _assemblies.constEnd(); ++assemiter)
		{
			Assembly* assembly = assemiter.value();
			_scene->addItem(assembly);
		}
	}

	Component* Page::currentComponent()
	{
		return _currentComponent;
	}

	void Page::setCurrentComponent(Component *currentComp)
	{
		if (currentComp == NULL)
		{
			_currentComponent = NULL; return;
		}

		if (currentComp == _currentComponent) return;

		if (_components.contains(currentComp->id()) == false)
		{
			addComponent(currentComp);
		}

		_currentComponent = currentComp;
	}

	QGraphicsItem* Page::getItemPtrById(QUuid id)
	{
		// NOTE - No check has been to verify if the item is of type
		// Component or Assembly
		// Only checking among components right now
		QHash<QUuid, Component*>::const_iterator componentiter;
		for (componentiter = _components.constBegin();
			 componentiter != _components.constEnd(); ++componentiter)
		{
			Component* component = componentiter.value();
			if (component->id() == id) return component;

			if (component->containsItem(id))
				return component->getItemById(id);
		}
		return NULL;
	}

	Component* Page::getComponentPtrById(QUuid id)
	{
		if (_components.contains(id))
			return _components.value(id);
		return NULL;
	}

	Assembly* Page::getAssemblyPtrById(QUuid id)
	{
		if (_assemblies.contains(id))
			return _assemblies.value(id);
		return NULL;
	}

	bool Page::contains(QUuid id)
	{
		// Check immediate objects
		if (_assemblies.contains(id)) return true;
		if (_components.contains(id)) return true;

		QHash<QUuid, Component*>::const_iterator componentiter;
		for (componentiter = _components.constBegin();
			 componentiter != _components.constEnd(); ++componentiter)
		{
			Component* component = componentiter.value();
			if (component->containsItem(id))
				return true;
		}

		// TODO - Check within assembly first
		return false;
	}

	ItemType Page::type() const
	{
		return PAGE;
	}

	Assembly* Page::createAssembly()
	{
		Assembly* newAssembly = new Assembly();
		addAssembly(newAssembly);
		return newAssembly;
	}

	Component* Page::createComponent()
	{
		Component* newComponent = new Component();
		addComponent(newComponent);
		return newComponent;
	}

	Component* Page::createComponent(Component* copy)
	{
		Component* newComponent = new Component(*copy);
		addComponent(newComponent);
		return newComponent;
	}

	void Page::addComponent(Component *component)
	{
		if (_components.contains(component->id())) return;
		_components.insert(component->id(), component);
		if (_scene) _scene->addItem(component);
		initializeComponentConnections(component);
		emit signalItemAdd(component);
	}

	void Page::addAssembly(Assembly *assembly)
	{
		if (_assemblies.contains(assembly->id())) return;
		_assemblies.insert(assembly->id(), assembly);
		if (_scene) _scene->addItem(assembly);
		emit signalItemAdd(assembly);
	}

	bool Page::mergeAssembly(Assembly *a1, Assembly *a2)
	{
		if (_assemblies.contains(a1->id()) && _assemblies.contains(a2->id()))
			return a1->mergeAssembly(a2);
		return false;
	}

	// This deletes the components as well
	bool Page::destroyAssembly(Assembly *assembly)
	{
		if (assembly == NULL) return false;
		if (_assemblies.contains(assembly->id()))
		{
			QList<Component*> lis_components = assembly->components();

			for (QList<Component*>::const_iterator iter = lis_components.constBegin();
				 iter != lis_components.constEnd(); ++iter)
			{
				Component* component = (*iter);
				_components.remove(component->id());
				assembly->removeComponent(component);
				delete component;
			}
			_assemblies.remove(assembly->id());
			delete assembly;
			return true;
		}
		return false;
	}

	bool Page::destroyComponent(Component *component)
	{
		// Do not delete the component if it is not contained in the page
		bool markForDelete = false;
		if (component == NULL)
		{
			QLOG_ERROR() << "Invalid component pointer";
			return false;
		}

		// 0. Remove the component from component hash
		if (_components.contains(component->id()))
		{
			markForDelete = true;
			_components.remove(component->id());
			removeComponentConnections(component);
		}

		// Remove it from temp list
		if (transformedComponents.contains(component))
		{
			transformedComponents.removeOne(component);
		}

		if (_currentComponent == component)
		{
			_currentComponent = NULL;
		}

		// 1. Find the assembly which contains the components
		QHash<QUuid, Assembly*>::const_iterator assemiter;
		for (assemiter = _assemblies.constBegin();
			 assemiter != _assemblies.constEnd(); ++assemiter)
		{
			Assembly* assembly = assemiter.value();
			if (assembly->containsComponent(component->id()))
			{
				markForDelete = true;
				assembly->removeComponent(component);
				break;
			}
		}
		if (markForDelete)
		{
			emit signalItemDelete(component);
			QList<QGraphicsItem*> childItems = component->childItems();
			b2Body* physicsBody = component->physicsBody();
			if (physicsBody)
			{
				foreach (auto* joint, component->_jointlist)
				{
					_physicsManager->deleteJoint(joint);
				}
				component->_jointlist.clear();

				foreach (b2Fixture* fixture, component->_fixtures)
				{
					physicsBody->DestroyFixture(fixture);
				}
				component->_fixtures.clear();

				_physicsManager->destroyBody(component);
				component->_physicsBody = NULL;
			}

			qDeleteAll(childItems);

			delete component;
		}
		return markForDelete;
	}

	bool Page::add(Page* page)
	{
		QLOG_TRACE() << "Feature not implemented";
		if (page== NULL) QLOG_INFO() <<"Not cool. NULL Page pointer@Page::add()";
		return false;
	}

	/*---------------------------------------------------------------------------------
	 * Scene query and paint related functions
	 * BEGIN
	 * -------------------------------------------------------------------------------*/
	QImage Page::getSelectionImage(QPolygonF selectionPolygon)
	{
		// Selection polygon should consist of at least 2 vertices
		if (selectionPolygon.size() < 3) return QImage();

		_scene->clearSelection();

		// Get the AABB of selection
		QRectF boundingBox = selectionPolygon.boundingRect();
		_scene->setSceneRect(_scene->itemsBoundingRect());

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
		QList<QGraphicsItem*> allitems = _scene->items();
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

	QImage Page::getSelectionImageFromHighlight()
	{
		QRect sceneRect = _scene->itemsBoundingRect().toRect();
		int x_min = sceneRect.right();
		int x_max = sceneRect.left();
		int y_min = sceneRect.bottom();
		int y_max = sceneRect.top();

		QImage image(sceneRect.x()+sceneRect.width(),
					 sceneRect.y()+sceneRect.height(),
					 QImage::Format_ARGB32_Premultiplied);
		image.fill(Qt::white);

		QPainter painter(&image);
		painter.setRenderHint(QPainter::Antialiasing, true);

		auto allitems = _scene->items();
		for (auto graphicsitem : allitems)	// Might return specific type in future
		{
			if (graphicsitem->type() == Stroke::Type && graphicsitem->isVisible())
			{
				Stroke* stroke = qgraphicsitem_cast<Stroke*>(graphicsitem);
				if (stroke->isHighlighted())
				{
					stroke->highlight(false);
					painter.setTransform(stroke->sceneTransform());
					stroke->paint(&painter, NULL);
					stroke->highlight(true);

					QRect rect= stroke->mapRectToScene(stroke->boundingRect()).toRect();
							//stroke->sceneTransform().inverted().mapRect(stroke->boundingRect()).toRect();
					x_min = (x_min < rect.x() ? x_min : rect.x());
					y_min = (y_min < rect.y() ? y_min : rect.y());

					x_max = (rect.right() > x_max) ? rect.right() : x_max;
					y_max = (rect.bottom() > y_max) ? rect.bottom() : y_max;
				}
			}
		}
		// Something spooky going on here. mostly ==
		if (x_min >= x_max || y_min >= y_max) return QImage();

		int diffX = x_max-x_min;
		int diffY = y_max-y_min;

		// Crop only what is needed
		QImage tightCropping = image.copy(QRect(x_min, y_min, diffX, diffY));

		int maxDim = ( (x_max-x_min) > (y_max-y_min) ? (x_max-x_min) : (y_max-y_min) );
		QImage squareImage = QImage(maxDim, maxDim, QImage::Format_ARGB32_Premultiplied);
		squareImage.fill(Qt::white);

		QPoint offset(0,0);
		if (maxDim > diffX) offset.setX((maxDim-diffX)/2);
		if (maxDim > diffY) offset.setY((maxDim-diffY)/2);
		QPainter squarePainter(&squareImage);
		squarePainter.drawImage(offset, tightCropping);
		return squareImage;
	}

	QImage Page::getSelectionImage()
	{
		//		if (freeStrokes.size() == 0 ) return QImage();
		QRectF rect = _scene->sceneRect();
		int x_min = 0;  int y_min = 0;
		int x_max = rect.x() + rect.width();
		int y_max = rect.y() + rect.height();
		QImage image(x_max,y_max, QImage::Format_ARGB32);
		image.fill(Qt::transparent);

		QPainter painter(&image);
		painter.setRenderHint(QPainter::Antialiasing);
		x_min = x_max; y_min = y_max; x_max = 0; y_max = 0;
		QList<QGraphicsItem*> allitems = _scene->items();
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

	QList<Stroke*> Page::getSelectedStrokes(Point2D pos, float margin)
	{
		QList<Stroke*> selectedStrokes = QList<Stroke*>();
		QList<QGraphicsItem*> allitems = _scene->items();

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

	QList<Stroke*> Page::getSelectedStrokes
	(QPolygonF selectionPolygon, float minimalAllowedSelection)
	{
		QList<Stroke*> selectedStrokes;
		QList<QGraphicsItem*> allitems = _scene->items();
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

	QList<Stroke*> Page::getHighlightedStrokes()
	{
		QList<Stroke*> selectedStrokes;
		QList<QGraphicsItem*> allitems = _scene->items();
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

	QList<QGraphicsItem *> Page::getSelectedItems(Point2D pos, float margin)
	{
		QList<QGraphicsItem*>  selectedItems;
		QList<QGraphicsItem*> allitems = _scene->items(pos, Qt::IntersectsItemBoundingRect,
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

	QRectF Page::getBoundingBox(QList<QGraphicsItem *> listOfItems)
	{
		if (listOfItems.size() == 0)
		{
			QLOG_ERROR() << "List of selected items is empty.";
			return QRectF();
		}
		// sceneTransform().inverted() does not work.. i think
		QRectF baseRect = listOfItems[0]->mapRectToScene(listOfItems[0]->boundingRect());
		for (int i=1; i < listOfItems.size(); i++)
		{
			baseRect = baseRect.united(
						listOfItems[i]->mapRectToScene(listOfItems[i]->boundingRect()));
		}
		return baseRect;
	}

	void Page::clearStrokeHighlight()
	{
		QList<QGraphicsItem*> allitems = _scene->items();
		for(QList<QGraphicsItem*>::const_iterator iter = allitems.constBegin();
			iter != allitems.constEnd(); ++iter)
		{
			QGraphicsItem* graphicsitem = (*iter);
			if (graphicsitem->type() != Stroke::Type) continue;
			Stroke* stroke = qgraphicsitem_cast<Stroke*>(graphicsitem);
			if (stroke->isHighlighted()) stroke->highlight(false);
		}
	}

	/*---------------------------------------------------------------------------------
	 * Scene query and paint related functions
	 * END
	 * -------------------------------------------------------------------------------*/

	QDataStream& Page::serialize(QDataStream& stream) const
	{
		stream << _id;
		// Make sure not to serialize components which have already been serialized by the assembly;
		// Implement saving of component data only
		int num_components = _components.size();
		int num_assemblies = _assemblies.size();
		stream << num_components;
		stream << num_assemblies;
		if (num_components == 0 && num_assemblies == 0) return stream;

		QHash<QUuid, Component*>::const_iterator componentiter;
		for (componentiter = _components.constBegin();
			 componentiter != _components.constEnd(); ++componentiter)
		{
			Component* component = componentiter.value();
			component->serialize(stream);
		}
		return stream;
	}

	QDataStream& Page::deserialize(QDataStream& stream)
	{
		QUuid newId;
		stream >> newId;

		_id = newId;

		int num_components = 0;
		int num_assemblies = 0;
		stream >> num_components;
		stream >> num_assemblies;		// Do not write assembly info now.

		if (num_components != 0)
		{
			for (int i=0; i<num_components; i++)
			{
				Component *component = new Component();
				component->deserialize(stream);
				addComponent(component);
			}
		}
		emit signalReloadPage(this);

		return stream;
	}

	void Page::deleteAll()
	{
		// Do not iterate over hash to delete items. get the list of objects and delete them
		QList<Assembly*> assemblies = _assemblies.values();
		for (int i=0; i < assemblies.size(); i++)
		{
			Assembly* assembly = assemblies[i];
			destroyAssembly(assembly);
		}
		_assemblies.clear();

		QList<Component*> components = _components.values();
		for (int i=0; i < components.size(); i++)
		{
			Component* component  = components[i];
			destroyComponent(component);
		}
		_components.clear();
	}

	void Page::initializeComponentConnections(Component *component)
	{
		// add other connections as well
		connect(component, SIGNAL(onTransformChange(QGraphicsItem*)),
				this, SLOT(onItemTransformUpdate(QGraphicsItem*)));
	}

	void Page::removeComponentConnections(Component *component)
	{
		Q_UNUSED(component)
	}

	// Slots
	void Page::onItemAdd(QGraphicsItem* graphicsitem)
	{
		emit signalItemAdd(graphicsitem);
	}

	void Page::onItemUpdate(QGraphicsItem* graphicsitem)
	{
		emit signalItemUpdate(graphicsitem);
	}

	void Page::onItemDelete(QGraphicsItem* graphicsitem)
	{
		emit signalItemDelete(graphicsitem);
	}

	void Page::onItemTransformUpdate(QGraphicsItem* graphicsitem)
	{
		if (graphicsitem->type() == Component::Type)
			onComponentTransformUpdate
					(qgraphicsitem_cast<Component*>(graphicsitem));
		emit signalItemTransformUpdate(graphicsitem);
	}

	void Page::onItemAdd(Item* item)
	{
		emit signalItemAdd(item);
	}

	void Page::onItemUpdate(Item* item)
	{
		emit signalItemUpdate(item);
	}

	void Page::onItemDelete(Item* item)
	{
		emit signalItemDelete(item);
	}

	void Page::onItemTransformUpdate(Item* item)
	{
		emit signalItemTransformUpdate(item);
	}

	void Page::onSimulationStepStart()
	{
		if (_physicsManager && transformedComponents.size())
		{
			_physicsManager->updateFromComponentPosition(transformedComponents);
			transformedComponents.clear();	// update the coords
		}
	}

	void Page::onSimulationStepComplete()
	{
		QList<Component*> lis_comps = _components.values();
		if (_physicsManager)
			_physicsManager->updateComponentPosition(lis_comps);
		if (_scene)	_scene->update();
	}

	void Page::onComponentTransformUpdate(Component *component)
	{
		// Batches the transformation behavior which updates the
		// box2d prior to step()
		if (transformedComponents.contains(component) == false)
			transformedComponents.push_back(component);
	}

	QDataStream& operator<<(QDataStream& stream, const Page& page)
	{
		return page.serialize(stream);
	}

	QDataStream& operator>>(QDataStream& stream, Page& page)
	{
		return page.deserialize(stream);
	}
}
