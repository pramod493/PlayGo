#pragma once
#include <QHash>
#include <QUuid>
#include "commonfunctions.h"
#include "component.h"

namespace CDI
{
	/**
	 * @brief The Assembly class contains a list of components
	 * NOTE: Unlike component object, it does not contain allow
	 * adding any AbstractModelItem object to itself.
	 * Also, this is a top-level object and inherits from AbstractModelItem
	 * only for ease of use
	 */
	class Assembly : public QHash<QUuid, Component*>, public AbstractModelItem
	{
	protected:
		// TODO - Not sure how to get this one to work;
		void* _rootPtr;				/**< store the pointer to the parent item */
		// TODO - This member is not used anywhere
		QString _assemblyFilepath;	/**< Stores the assembly file path */
		QTransform _transform;

	public:
		Assembly();

		~Assembly();

		QTransform transform() const;

		inline void setTransform(QTransform& transform);

		ItemType type() const;
		void addItem(Component* component);

		void removeItem(Component* component);
		void removeItem(QUuid uid);
		void deleteitem(Component* component);

		virtual bool containsItem(AbstractModelItem* key, bool searchRecursive = false);
		virtual bool containsItem(QUuid key, bool searchRecursive = false);
		virtual bool containsItem(QString key, bool searchRecursive = false);

		/**
		 * @brief ReadFromFile: Reads assembly information from given file.
		 * Usually, everything will be saved under a common root object but
		 * this allows the option to save data into individual files
		 * @param file: File to read
		 * @return True, if success. False otherwise
		 */
		bool readFromFile(QFile& file);

		/**
		 * @brief WriteToFile: Writes the assembly information to given file
		 * This includes adding extra header and item type information for reading
		 * @param file: Save target location
		 * @return True, if success. False otherwise
		 */
		bool writeToFile(QFile& file);

		/**
		 * @brief serialize: Serialization for Assembly and contained objects
		 * @param stream
		 * @return
		 */
		QDataStream& serialize(QDataStream& stream) const ;

		/**
		 * @brief deserialize
		 * @param stream
		 * @return
		 */
		QDataStream& deserialize(QDataStream& stream);
	};

	/******************************************************
	 * Assembly inline functions
	 *****************************************************/
	inline void Assembly::setTransform(QTransform &transform)
	{
		mask |= isTransformed;
		_transform = transform;
	}
}
