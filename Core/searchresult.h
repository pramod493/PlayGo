#pragma once
#include "abstractmodelitem.h"
#include <QString>

namespace CDI
{
	/**
	 * @brief The SearchResult class contains information related to search results.
	 * It does not have any transformation
	 */
    class SearchResult : public Item
	{
	public:
		QString resultFilePath;	/**< Absolute file path of the result*/
		bool metadataExists;		/**< Set to true if metadata exists*/
		QStringList tags;			/**< Stores the list of all the tags*/

    protected:
        QUuid _id;
	public:
		SearchResult();
        SearchResult(QString& resultImagePath);
		SearchResult(const SearchResult& result);
        virtual ~SearchResult();

        QUuid id() const;

        ItemType type() const;

        QDataStream& serialize(QDataStream &stream) const;
        QDataStream& deserialize(QDataStream &stream);
	};
}
