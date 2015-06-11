#include "playgo.h"

namespace CDI
{
	PlayGo::PlayGo()
	{
		_pages = QHash<QUuid, Page*>();
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
				if (page->contains(id, searchRecursive))
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
		if (_pages.contains(id)) return _pages.value(id);	// Item is Page

		QHash<QUuid, Page*>::const_iterator iter;
		for (iter = _pages.constBegin();
			 iter != _pages.constEnd(); ++iter)
		{
			Page* page = iter.value();
			if (page->contains(id, searchRecursive))
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
		stream << _pages.size();
		QHash<QUuid, Page*>::const_iterator iter;
		for (iter = _pages.constBegin(); iter != _pages.constEnd(); ++iter)
		{
			Page*  page= iter.value();
			stream << *page;
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
		int num_pages;
		stream >> num_pages;
		for (int i=0; i< num_pages; i++)
		{
			Page *page = new Page(this);
			stream >> *page;
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

	QDataStream& operator>>(QDataStream& stream, const PlayGo& item)
	{
		return item.serialize(stream);
	}

	QDataStream& operator<<(QDataStream& stream, PlayGo& item)
	{
		return item.deserialize(stream);
	}
}

