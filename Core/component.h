#pragma once
#include <QHash>
#include <QTransform>
#include "commonfunctions.h"
#include "abstractmodelitem.h"
#include "stroke.h"
#include "image.h"

namespace CDI
{
	class Page;
	class Assembly;
	class PhysicsManager;
	// This class can contain Component objects as well
	// TODO - Destructor should delete all the items as well
	// Do not add Assembly objects to this.
	// Limit types to COMPONENT, STROKE, IMAGE, PHYSICS..., POLYGON2D
	class Component : public Item, public QHash<QUuid, AbstractModelItem*>
	{
	public:
		unsigned int mask;

	protected:
		QUuid _id;

		Page* _pagePtr;

		Assembly* _assemblyParent;

		QTransform _transform;

		QTransform _inverseTransform;

		Point2D _position;

		float _scale;	// Force uniform scaling along x and y

		float _rotation;

		bool _transformModified;

		/**
		 * @brief Mark as true whenever object is scaled.
		 * @remarks Scaling of component requires re-creation of b2body shape and therefore is important
		 */
		bool _componentScaled;

	public:
		//-----------------------------------------------
		// Constructors/Destructors
		//-----------------------------------------------
		/**
		 * @brief Component : Creates Component object. Private constructor prevents
		 * creation by any other entities. Create and delete operation handled by Page
		 * or Assembly object (declared as friend to this class.
		 */
		Component(Page* parent);

		/**
		 * @brief ~Component handles deletion of Component object. It also manages the
		 * deletion of contained AbstractModelItem objects.
		 */
		virtual ~Component();

		//-----------------------------------------------
		// Query/Set functions(same order in derived class)
		// Non-virtual
		//-----------------------------------------------
		QUuid id() const;

		Assembly* parentAssembly();

		void setParentAssembly(Assembly* assembly);

		//-----------------------------------------------
		// Virtual functions (same order in derived class)
		//-----------------------------------------------
		/**
		 * @brief Returns COMPONENT
		 * @return COMPONENT
		 */
		ItemType type() const;

		virtual Point2D position();

		virtual float scale();

		virtual float rotation();

		virtual void setPosition(float x, float y);

		virtual void translateBy(float x, float y);

		virtual void translateTo(float x, float y);

		virtual void setRotation(float rot);

		virtual void rotateBy(float deltaRot);

		virtual void scaleBy(float scaleFactor);

		/**
		 * @brief update item id reference on change in item ID. This happens when the item is read after being created
		 * @param oldID Old Item QUuid
		 * @param newID New Item QUuid
		 */
		virtual void onIdUpdate(QUuid oldID, QUuid newID);

		/**
		 * @brief Returns component transform
		 * @return
		 */
		virtual QTransform transform();

		/**
		 * @brief Returns the global transformation of this Component w.r.t. scene
		 * @return Global transform
		 */
		virtual QTransform globalTransform();

		/**
		 * @brief Get transformation w.r.t. to given component
		 * @param itemId
		 * @return Absolute transform of given item.
		 */
		virtual QTransform itemTransform(AbstractModelItem*);

		/**
		 * @brief Get transformation w.r.t. to given component
		 * @param itemId:QUuid of the item
		 * @return Absolute transform of given item. Returns transform of
		 * first item in case of multiple matches
		 */
		virtual QTransform itemTransform(QUuid itemId);

		/**
		 * @brief setTransform sets the Component transform
		 * @param transform: New transformation
		 * @todo Must be able to extract position, rotation and scale values from transform
		 * @remarks This also extracts the rotation, scaling and translation values
		 * and might be computationally expensive. Use with caution
		 */
		virtual void setTransform(QTransform& transform, bool combine = false);

		/**
		 * @brief Creates a new Stroke and adds it to the component
		 * @param color Stroke color
		 * @param thickness Stroke thickness
		 * @return Pointer to created Stroke
		 */
		virtual Stroke* addStroke(QColor color = Qt::black, float thickness = 1.0f);

		/**
		 * @brief Creates a new Stroke and adds it to the component
		 * @param points List of points to add to the Stroke
		 * @param color Stroke color
		 * @param thickness Stroke thickness
		 * @return Pointer to created Stroke
		 */
		virtual Stroke* addStroke(const QVector<Point2DPT>& points, QColor color, float thickness);

		/**
		 * @brief Creates and adds an empty Image to the Component
		 * @return pointer to created Image object
		 */
		virtual Image* addImage();

		/**
		 * @brief Creates an Image object based on the given filepath
		 * @param filename Filepath containing Image information (primarily png file)
		 * @return Pointer to created Image object
		 */
		virtual Image* addImage(const QString filename);

		/**
		 * @brief Creates an Image object based on the given QPixmap and filepath. It sets the QPixmap
		 * and filepath values to the input arguments
		 * @param pixmap QPixmap reference
		 * @param filename Filename for Image
		 * @return Pointer to created Image object
		 */
		virtual Image* addImage(const QPixmap &pixmap, QString filename);

		/**
		 * @brief Adds the given item to the hash
		 * @param item AbstractModelItem pointer
		 */
		virtual void addItem(AbstractModelItem* item);

		/**
		 * @brief Removes the reference of the item from the Component. Don't set it parent to NULL.
		 * @param item Item to remove from component
		 * @return True if item was found in the object, false otherwise
		 */
		virtual bool removeItem(AbstractModelItem* item);

		/**
		 * @brief Removes the reference of the item from the Component. Don't set it parent to NULL.
		 * @param itemId ID of item to remove from component
		 * @return True if item was found in the object, false otherwise
		 */
		virtual bool removeItem(QUuid itemId);

		// Query functions.
		// Use these instead of default query functions of QHash fo extra features
		// NOTE: Avoid searchRecursive option unless required. Affect on performance
		// Since recursive parenting is disabled, searchRecursive is of no use
		// not measured.
		virtual bool containsItem(AbstractModelItem* key, bool searchRecursive = false);
		virtual bool containsItem(QUuid key, bool searchRecursive = false);
		virtual bool containsItem(QString key, bool searchRecursive = false);

		virtual AbstractModelItem* getItemPtrById(QUuid key/*, bool searchRecursive = false*/);

		virtual QDataStream& serialize(QDataStream& stream) const;
		virtual QDataStream& deserialize(QDataStream& stream);

	protected:
		// If new item types are added, extend this class to accomodate those
		virtual AbstractModelItem* createEmptyItem(ItemType itemtype);

		virtual bool isItemTypeSupported(ItemType t) const;

		void updateTransform();

	public /*slots*/:
		/**
		 * @brief Called from AbstractModelItem destructor. This removes the item's reference
		 * from the component as well as performs other tasks including updating the display
		 * and physics component
		 * @param item Item pointer
		 */
		void onItemDelete(AbstractModelItem* item);

		/**
		 * @brief Called from AbstractModelItem destructor. This removes the item's reference
		 * from the component as well as performs other tasks including updating the display
		 * and physics component
		 * @param itemKey Item QUuid
		 */
		void onItemDelete(QUuid itemKey);

		/**
		 * @brief Called when AbstractModelitem is updated. This causes re-rendering of the object
		 * and use 'mask' to narrow down the changes
		 * @param item Item pointer
		 */
		void onItemUpdate(AbstractModelItem* item);
		/**
		 * @brief Called when AbstractModelitem is updated. This causes re-rendering of the object
		 * and use 'mask' to narrow down the changes
		 * @param itemKey Item QUuid
		 */
		void onItemUpdate(QUuid itemKey);

		void onItemDisplayUpdate(AbstractModelItem* item);
		void onItemDisplayUpdate(QUuid itemKey);

		/**
		 * @brief Called when AbstractModelItem transform is updated
		 * @param item Item pointer
		 */
		void onItemTransformUpdate(AbstractModelItem* item);

		/**
		 * @brief called when AbstractModelItem transform is uodated.
		 * @param itemKey Item QUuid
		 */
		void onItemTransformUpdate(QUuid itemKey);

		friend class Page;
		friend class Assembly;	// Try to make it work without allowing assembly to
		friend class PhysicsManager;
	};
}
