#pragma once
#include "abstractmodelitem.h"
#include <QString>

namespace CDI
{
	/**
	 * @brief The SearchResult class contains information related to search results.
	 * It does not have any transformation
	 */
	class SearchResult
	{
	public:
		QString resultFilePath;	/**< Absolute file path of the result*/
		bool metadataExists;		/**< Set to true if metadata exists*/
		QStringList tags;			/**< Stores the list of all the tags*/
	public:
		SearchResult();
		SearchResult(const SearchResult& result);
		virtual ~SearchResult();

		virtual ItemType type() const;

		virtual QDataStream& serialize(QDataStream &stream) const;
		virtual QDataStream& deserialize(QDataStream &stream);
	};
}
