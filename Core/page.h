#pragma once
#include <QDataStream>
#include <QObject>
#include <QList>
#include <QUuid>
#include <QtAlgorithms>
#include <QGraphicsScene>

#include "commonfunctions.h"
#include "assembly.h"
#include "component.h"
#include "searchmanager.h"
#include "physicsmanager.h"

using namespace boost;

namespace CDI
{
	class PlayGo;

	/**
	 * @brief The Page class is the top level item which contains self sufficient information regarding a scene.
	 * It also serves the function of factory class to create objects
	 * This is the top-level container for all AbstractModelItem objects
	 */
	class Page : public QObject //, public Item
	{
		Q_OBJECT

	protected:
		/**
		 * @brief Scene which will be used for rendering and all other operations
		 */
		QGraphicsScene* _scene;

		/**
		 * @brief Pointer to root item of the model
		 */
		PlayGo *_playgo;

		/**
		 * @brief Page object ID
		 */
		QUuid _id;

		/**
		 * @brief _assemblies contains all the Assembly objects
		 * contained within the page.
		 */
		QHash<QUuid, Assembly*> _assemblies;

		/**
		 * @brief _components contains all the Component objects
		 * contained within the page including all the components
		 * which are part of a given Assembly
		 */
		QHash<QUuid, Component*> _components;

		/**
		 * @brief Pointer to search manager for this page
		 */
		SearchManager* _searchManager;

		/**
		 * @brief Pointer to physics manager for this page
		 */
		PhysicsManager* _physicsManager;

		Component* _currentComponent;

		QList<Component*> transformedComponents;

		Component* static_component;

		cdLayerIndex _currentLayerIndex;

	protected :
		void setScene(QGraphicsScene* scene);	// Do not allow changing of scene

	public:
		//-----------------------------------------------
		// Constructors/Destructors
		//-----------------------------------------------
		Page(PlayGo* parent = NULL);

		Page(const Page& page);

		virtual ~Page();

		//-----------------------------------------------
		// Query/Set functions(same order in derived class)
		// Non-virtual
		//-----------------------------------------------
		virtual QUuid id() const;

		SearchManager* getSearchManager() const;

		PhysicsManager* getPhysicsManager() const;

		QList<Assembly*> getAssemblies() const;

		QList<Component*> getComponents() const;

		QGraphicsScene* scene() const;

		Component* currentComponent();

		void setCurrentComponent(Component* currentComp);

		void setCurrentLayer(cdLayerIndex newindex);

		cdLayerIndex currentLayer() const;

		QGraphicsItem* getItemPtrById(QUuid id);

		Component* getComponentPtrById(QUuid id);	// These are also QGraphicsItem objects

		Assembly* getAssemblyPtrById(QUuid id);

		/**
		 * @brief Checks if the given item is contained in the Page
		 * @param id
		 * @param searchRecursive
		 * @return
		 */
		bool contains(QUuid id);

		//-----------------------------------------------
		// Virtual functions (same order in derived class)
		//-----------------------------------------------
		virtual ItemType type() const;

		virtual Assembly* createAssembly();
		virtual Component* createComponent();
		virtual Component* createComponent(Component* copy);

		virtual void addComponent(Component* component);
		virtual void addAssembly(Assembly* assembly);

		/**
		 * @brief Merges all objects in a2 into a1 and deletes a2
		 * @param a1
		 * @param a2
		 * @return
		 */
		virtual bool mergeAssembly(Assembly* a1, Assembly* a2);

		/**
		 * @brief Deletes all the components stored in an assembly as well
		 * as deletes the assembly.
		 * @param assembly
		 * @return True if deletion is successfull. False when the assembly
		 * is not a part of the page
		 */
		virtual bool destroyAssembly(Assembly* assembly);

		/**
		 * @brief Deletes all the items stored in the component as well
		 * as deletes the assembly.
		 * @param component
		 * @return True if deletion is successfull. False when the component
		 * is not a part of the page
		 */
		virtual bool destroyComponent(Component* component);

		/**
		 * @brief add copies the given page information into this page
		 * @param page: Page to add
		 * @return True on success, false on fail
		 */
		virtual bool add(Page* page);

		/*---------------------------------------------------------------------------------
		 * Scene query and paint related functions
		 * BEGIN
		 * -------------------------------------------------------------------------------*/
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
		 * @brief Find the components at given position. Note that this checks against visible polygons
		 * as well as images and strokes to select components
		 * @param pos
		 * @param margin
		 * @return
		 */
		QList<QGraphicsItem *> getSelectedItems(Point2D pos, float margin = 0);

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

		void clearStrokeHighlight();

		Component* getStaticPart();

		/*---------------------------------------------------------------------------------
		 * Scene query and paint related functions
		 * END
		 * -------------------------------------------------------------------------------*/

		/**
		 * @brief Serializes page and its component
		 * @param stream
		 * @return
		 */
		virtual QDataStream& serialize(QDataStream &stream) const;

		/**
		 * @brief Deserializes page and re-creates components
		 * @param stream
		 * @return
		 */
		virtual QDataStream& deserialize(QDataStream &stream);

		//-----------------------------------------------
		// Other functions not related to query/set
		//-----------------------------------------------
		/**
		 * @brief Deletes all components and assemblies
		 */
		void deleteAll();

		// These do not have to friend function
		friend QDataStream& operator<<(QDataStream& stream, const Page& page);
		friend QDataStream& operator>>(QDataStream& stream, Page& page);

	protected:
		void initializeComponentConnections(Component* component);

		void removeComponentConnections(Component* component);

		// These function are called on item updates
		// Note that if a component or assembly does not exist in the hash list
		// it's update signal will be discarded
	public slots:
		void onItemAdd(QGraphicsItem* graphicsitem);
		void onItemUpdate(QGraphicsItem* graphicsitem);
		void onItemDelete(QGraphicsItem* graphicsitem);
		void onItemTransformUpdate(QGraphicsItem* graphicsitem);
		void onItemAdd(Item* item);
		void onItemUpdate(Item* item);
		void onItemDelete(Item* item);
		void onItemTransformUpdate(Item* item);

		void onSimulationStepStart();
		void onSimulationStepComplete();

		void onComponentTransformUpdate(Component* component);

	signals:
		void signalReloadPage(Page* page);

		void signalItemAdd(QGraphicsItem* graphicsitem);
		void signalItemUpdate(QGraphicsItem* graphicsitem);
		void signalItemDelete(QGraphicsItem* graphicsitem);
		void signalItemTransformUpdate(QGraphicsItem* graphicsitem);

		void signalItemAdd(Item* item);
		void signalItemUpdate(Item* item);
		void signalItemDelete(Item* item);
		void signalItemTransformUpdate(Item* item);
	};
}

Q_DECLARE_METATYPE(CDI::Page)
