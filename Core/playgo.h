#pragma once
#include <QHash>
#include <QObject>
#include <QtAlgorithms>
#include <QList>
#include "commonfunctions.h"
#include "abstractmodelitem.h"
#include "component.h"
#include "page.h"

/**
 \mainpage PlayGo core component documentation
 \section Introducion to datastructure
 All objetcs are derived from AbstractModelItem object
 \section Conact Info
Author: Pramod Kumar
email: pramod@purdue.edu
email: pramod493@gmail.com
 */
namespace CDI
{
	/**
	 * @brief
	 * The root object which contains all the components.
	 * Mostly used for loading/saving files and maintaining ID unique
	 * This class behaves as factory class for creating pages
	 */
	class PlayGo : public QObject
	{
	protected:
		QHash<QUuid, Page*> _pages;
		Page* _currentPage;
	public:
		PlayGo();

		/**
		 * @brief Delete all the components in the scene and destroys the object
		 */
		~PlayGo();

		/**
		 * @brief deleteAllItems: Clear all items in the database
		 */
		void deleteAllItems();

		Page* currentPage();

		void setCurrentPage(Page* page);

		/**
		 * @brief addNewPage creates a new page and adds it to the object
		 * @return Newly created Page object
		 */
		virtual Page* addNewPage();
		/**
		 * @brief addPage adds an existing page to the object
		 * @param page
		 */
		virtual void addPage(Page* page);

		QList<Page*> values() const { return _pages.values(); }

		/**
		 * @brief contains checks if the given id is contained in the list.
		 * It will also search for the item in children if searchRecursive flag
		 * is true
		 * @param id
		 * @param searchRecursive
		 * @return
		 */
		virtual bool contains(QUuid id, bool searchRecursive);
		virtual bool contains(QString id, bool searchRecursive);		// Do not use this one

		virtual Page* mergePages(QVector<Page*> pagesToAdd);

		/**
		 * @brief getItemPage returns the page which contains a given item
		 * @param id: Item's ID
		 * @param searchRecursive: Perform deep search into components
		 * @return parent Page if available, NULL otherwise
		 */
		virtual Page* getItemPage(QUuid id, bool searchRecursive);

		virtual Page* getPageById(QUuid id);
		virtual Page* getPageById(QString id);

		QDataStream& serialize(QDataStream& stream) const;
		QDataStream& deserialize(QDataStream& stream);

	signals:
	public slots:

		friend QDataStream& operator<<(QDataStream& stream, const PlayGo& item);
		friend QDataStream& operator>>(QDataStream& stream, PlayGo& item);
	};
}
