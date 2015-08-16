#include "playgo.h"
#include "QsLog.h"

namespace CDI
{
	PlayGo::PlayGo()
	{
		_currentPage = NULL;
		_pages = QHash<QUuid, Page*>();
		_rootID = uniqueHash();
	}

	PlayGo::~PlayGo()
	{
		deleteAllItems();
	}

	void PlayGo::deleteAllItems()
	{
		// delete all components as well as free items
		if (!_pages.isEmpty())
		{
			QHash<QUuid, Page*>::const_iterator iter;
			for (iter = _pages.constBegin();
				 iter != _pages.constEnd(); ++iter)
			{
				Page* page= iter.value();
				if (page != NULL) delete page;
			}
		}
	}

	Page* PlayGo::currentPage()
	{
		return _currentPage;
	}

	void PlayGo::setCurrentPage(Page *page)
	{
		// Do not check if _currentPage is null or not.
		_currentPage = page;
	}

	Page* PlayGo::addNewPage()
	{
		Page* page = new Page(this);
		_pages.insert(page->id(), page);
		return page;
	}

	void PlayGo::addPage(Page* page)
	{
		if (!_pages.contains(page->id()))
			_pages.insert(page->id(), page);
	}

	bool PlayGo::contains(QUuid id, bool searchRecursive)
	{
		if (_pages.contains(id)) return true;
		if (searchRecursive)
		{
			QHash<QUuid, Page*>::const_iterator iter;
			for (iter = _pages.constBegin();
				 iter != _pages.constEnd(); ++iter)
			{
				Page* page = iter.value();
				if (page->contains(id))
					return true;
			}
		}
		return false;
	}

	bool PlayGo::contains(QString id, bool searchRecursive)
	{
		QUuid uid = QUuid(id);
		if (uid.isNull()) return false;
		return contains(uid, searchRecursive);
	}

	Page* PlayGo::mergePages(QVector<Page *> pagesToAdd)
	{
		Page *newpage = new Page(this);
		for (QVector<Page*>::iterator iter = pagesToAdd.begin();
			 iter != pagesToAdd.end(); ++iter)
			newpage->add(*iter);
		_pages.insert(newpage->id(), newpage);
		return newpage;
	}

	Page* PlayGo::getItemPage(QUuid id, bool searchRecursive)
	{
		Q_UNUSED(searchRecursive)
		if (_pages.contains(id)) return _pages.value(id);	// Item is Page

		QHash<QUuid, Page*>::const_iterator iter;
		for (iter = _pages.constBegin();
			 iter != _pages.constEnd(); ++iter)
		{
			Page* page = iter.value();
			if (page->contains(id))
				return page;
		}
		return NULL;
	}

	Page* PlayGo::getPageById(QUuid id)
	{
		if (_pages.contains(id))
		{
			return _pages.value(id);
		}
		return NULL;
	}

	Page* PlayGo::getPageById(QString id)
	{
		QUuid uid = QUuid(id);
		if (uid.isNull()) return false;
		return getPageById(uid);
	}

	QDataStream& PlayGo::serialize(QDataStream& stream) const
	{
		stream << _rootID;

		int num_pages = _pages.size();
		stream << num_pages;

		QHash<QUuid, Page*>::const_iterator iter;
		for (iter = _pages.constBegin(); iter != _pages.constEnd(); ++iter)
		{
			Page*  page= iter.value();
			page->serialize(stream);
		}
		// Mark for setting current page
		if (_currentPage== NULL)
		{
			stream << 0;
		} else
		{
			stream << 1;
			stream << _currentPage->id();
		}
		return stream;
	}

	QDataStream& PlayGo::deserialize(QDataStream& stream)
	{
		QUuid tmpId; stream >> tmpId;
		_rootID = tmpId;

		int num_pages;
		stream >> num_pages;

		for (int i=0; i< num_pages; i++)
		{
			Page *page = new Page(this);
			page->deserialize(stream);
			_pages.insert(page->id(), page);
		}
		int i;	stream >> i;
		if (i == 1)
		{
			QUuid currentPageId;
			stream >> currentPageId;
			if (_pages.contains(currentPageId))
				_currentPage = _pages.value(currentPageId);
		} else if (i==0)
		{
			_currentPage = NULL;
		}
		return stream;
	}

	bool PlayGo::SaveModel(QString filePath)
	{
		QFile file(filePath);
		if (file.open(QIODevice::WriteOnly))
		{
			QDataStream stream(&file);
			stream << VERSION_STR;
			serialize(stream);
			file.close();
			return true;
		} else {
			QLOG_INFO() << "Error in opening the file";
		}
		return false;
	}

	bool PlayGo::ReadModel(QString filePath)
	{
		QFile file(filePath);
		if (file.open(QIODevice::ReadOnly))
		{
			QDataStream stream(&file);
			int i;
			stream >> i;
			if (i!= VERSION_STR)
			{
				QLOG_INFO() << "Version not supported";
				file.close();
				return false;
			}
			deserialize(stream);
			file.close();
			return true;
		} else {
			QLOG_INFO() << "Error in opening the file";
		}
		return false;
	}

//	QDataStream& operator>>(QDataStream& stream, const PlayGo& item)
//	{
//		return item.serialize(stream);
//	}

//	QDataStream& operator<<(QDataStream& stream, PlayGo& item)
//	{
//		return item.deserialize(stream);
//	}
}

