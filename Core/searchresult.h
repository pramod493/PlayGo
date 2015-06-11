#pragma once
#include "abstractmodelitem.h"
#include <QString>

namespace CDI
{
	/**
	 * @brief The SearchResult class contains information related to search results.
	 * It does not have any transformation
	 */
	class SearchResult : public AbstractModelItem
	{
	public:
		QString resultFilePath;	/**< Absolute file path of the result*/
		bool metadataExists;		/**< Set to true if metadata exists*/
		QStringList tags;			/**< Stores the list of all the tags*/
	public:
		SearchResult();
		SearchResult(QObject *parent);
		SearchResult(const SearchResult& result);
		~SearchResult();

		ItemType type() const;

		QDataStream& serialize(QDataStream &stream) const;
		QDataStream& deserialize(QDataStream &stream);
	};
}

Q_DECLARE_METATYPE(CDI::SearchResult)
