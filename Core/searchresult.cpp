#include "searchresult.h"

namespace CDI
{
	SearchResult::SearchResult()
	{
		_id = uniqueHash();
		resultFilePath = "";
		metadataExists = false;
		tags = QStringList();
	}

	SearchResult::SearchResult(QString& resultImagePath)
	{
		_id = uniqueHash();
		resultFilePath = resultImagePath;
		metadataExists = false;
		tags = QStringList();
	}

	SearchResult::SearchResult(const SearchResult &result)
	{
		_id = uniqueHash();
		resultFilePath = result.resultFilePath;
		metadataExists = result.metadataExists;
		tags = result.tags;
	}

	SearchResult::~SearchResult()
	{
		// Nothing to delete here. Does not have anythin on stack
	}

	QUuid SearchResult::id() const
	{
		return _id;
	}

	ItemType SearchResult::type() const
	{
		return SEARCHRESULT;
	}

	QDataStream& SearchResult::serialize(QDataStream& stream) const
	{
		stream << _id;
		stream << resultFilePath;
		stream << metadataExists;
		stream << tags;
		return stream;
	}

	QDataStream& SearchResult::deserialize(QDataStream& stream)
	{
		stream >> _id;
		stream >> resultFilePath;
		stream >> metadataExists;
		stream >> tags;
		return stream;
	}

}
