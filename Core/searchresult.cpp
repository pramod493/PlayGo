#include "searchresult.h"

namespace CDI
{
	SearchResult::SearchResult()
	{
		resultFilePath = QString();
		metadataExists = false;
		tags = QStringList();
	}

	SearchResult::SearchResult(QObject *parent)
		: AbstractModelItem(parent)
	{
		resultFilePath = QString();
		metadataExists = false;
		tags = QStringList();
	}

	SearchResult::SearchResult(const SearchResult &result)
	{
		resultFilePath = result.resultFilePath;
		metadataExists = result.metadataExists;
		tags = result.tags;
	}

	SearchResult::~SearchResult()
	{

	}

	ItemType SearchResult::type() const
	{
		return ItemType::SEARCHRESULT;
	}

	QDataStream& SearchResult::serialize(QDataStream& stream) const
	{
		stream << resultFilePath;
		stream << metadataExists;
		stream << tags;
		return stream;
	}

	QDataStream& SearchResult::deserialize(QDataStream& stream)
	{
		stream >> resultFilePath;
		stream >> metadataExists;
		stream >> tags;
		return stream;
	}

}
