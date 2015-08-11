#include "filesystemwatcher.h"
#include <QDir>

namespace CDI
{
	FileSystemWatcher::FileSystemWatcher(QObject *parent)
		: QFileSystemWatcher(parent)
	{

	}

	void FileSystemWatcher::setDirectory(QString &dirname)
	{
		QDir tmpDir(dirname);
		if (tmpDir.exists())
		{
			// Assume no child directory. This might not work when contains nested dirs
			addPath(dirname);
			_watchedDir = dirname;
			_fileList = tmpDir.entryList
					(QDir::NoDotAndDotDot | QDir::System
					 | QDir::Hidden  | QDir::AllDirs
					 | QDir::Files, QDir::DirsFirst);
			connect(this, SIGNAL(directoryChanged(QString)),
					this, SLOT(onInternalDirectoryChange(QString)));
		}
	}

	void FileSystemWatcher::onInternalDirectoryChange(const QString &dir)
	{
		if (dir != _watchedDir) return;	// Do nothing. Folder not watched
		QDir tmpDir(_watchedDir);
		QStringList updatedFileList = tmpDir.entryList
				(QDir::NoDotAndDotDot | QDir::System
				 | QDir::Hidden  | QDir::AllDirs
				 | QDir::Files, QDir::DirsFirst);

		if (updatedFileList.size() < _fileList.size())
		{
			// File deleted
			_fileList = updatedFileList;
		} else if (updatedFileList.size() > _fileList.size())
		{
			// File added. Check with each item in the updatedList with old list
			for (int i=0; i< updatedFileList.size(); i++)
			{
				QString tmpFile = updatedFileList[i];
				if (_fileList.contains(tmpFile, Qt::CaseInsensitive) == false)
				{
					// This file was added.
					_fileList.push_back(tmpFile);
					emit fileAdded(tmpDir.absoluteFilePath(tmpFile));
				}
			}
		} else
		{
			// File changed/renamed
			_fileList = updatedFileList;
		}
	}
}
