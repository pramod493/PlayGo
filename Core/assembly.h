#pragma once
#include <QHash>
#include <QMultiHash>
#include <QUuid>
#include "commonfunctions.h"
#include "component.h"

namespace CDI
{
	class Page;

	typedef QHash<QUuid, Item*> ItemHash;
	/**
	 * @brief The Assembly class contains a list of components
	 * NOTE: Unlike component object, it does not contain allow
	 * adding any AbstractModelItem object to itself.
	 * Also, this is a top-level object and inherits from AbstractModelItem
	 * only for ease of use
	 * It contains other componets which enable creation of linkages between
	 * Components.physicsBody
	 * @remark (17 June, 2015) Let the Assembly manage a list of all the
	 * items which form part of the assembly. Note that not all CDI::Item are
	 * allowed in the assembly and therefore limit addition of items to QHash
	 */
	class Assembly : public Item, protected ItemHash
	{

	public:
		unsigned int mask;

	protected:
		QUuid _id;

		Page* _pagePtr;				/**< store the pointer to the parent item */

		// TODO - This member is not used anywhere
		QTransform _transform;

		// TODO In assembly we want to create an adjancency map between physics bodies and joints

	public:
		//-----------------------------------------------
		// Expose selected memebers from protected base class
		//-----------------------------------------------
		using ItemHash::contains;
		using ItemHash::value;
		using ItemHash::values;
		using ItemHash::size;
		using ItemHash::constBegin;
		using ItemHash::constEnd;

		//-----------------------------------------------
		// Constructors/Destructors
		//-----------------------------------------------
		/**
		 * @brief Assembly : Creates Assembly object. Private constructor prevents
		 * creation by any other entities. Create and delete operation handled by CDI::Page
		 */
		Assembly(Page* parent);

		/**
		 * @brief ~Assembly handles deletion of Assembly object. It does not affect
		 * the contained Component objects, only the assembly object is deleted. The deletion
		 * of Components is handled by Page.
		 */
		virtual ~Assembly();

		//-----------------------------------------------
		// Query/Set functions(same order in derived class)
		// Non-virtual
		//-----------------------------------------------
		/**
		 * @brief Returns the Assembly object ID
		 * @return Assembly ID
		 */
		QUuid id() const;

		//-----------------------------------------------
		// Virtual functions (same order in derived class)
		//-----------------------------------------------
		/**
		 * @brief Returns the ItemType of object
		 * @return ASSEMBLY
		 */
		ItemType type() const;

		/**
		 * @brief transform returns identity matrix
		 * @return
		 */
		virtual QTransform transform() const;

		/**
		 * @brief setTransform should should update the assembly id
		 * @param transform
		 */
		virtual void setTransform(QTransform transform);


		void addItem(Component* component);

		void removeItem(Component* component);
		void removeItem(QUuid uid);

		virtual bool containsItem(AbstractModelItem* key, bool searchRecursive = false);
		virtual bool containsItem(QUuid key, bool searchRecursive = false);
		virtual bool containsItem(QString key, bool searchRecursive = false);

		//		/**
		//		 * @brief ReadFromFile: Reads assembly information from given file.
		//		 * Usually, everything will be saved under a common root object but
		//		 * this allows the option to save data into individual files
		//		 * @param file: File to read
		//		 * @return True, if success. False otherwise
		//		 */
		//		bool readFromFile(QFile& file);
		//
		//		/**
		//		 * @brief WriteToFile: Writes the assembly information to given file
		//		 * This includes adding extra header and item type information for reading
		//		 * @param file: Save target location
		//		 * @return True, if success. False otherwise
		//		 */
		//		bool writeToFile(QFile& file);

		/**
		 * @brief serialize: Serialization for Assembly and contained objects
		 * @param stream
		 * @return
		 */
		virtual QDataStream& serialize(QDataStream& stream) const;

		/**
		 * @brief deserialize
		 * @param stream
		 * @return
		 */
		virtual QDataStream& deserialize(QDataStream& stream);

	protected:
		bool mergeAssembly(Assembly* assembly);// { Q_UNUSED(assembly) return false; }

		friend class Page;
	};
}
