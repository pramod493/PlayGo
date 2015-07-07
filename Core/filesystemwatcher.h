#pragma once
#include <QFileSystemWatcher>
#include <QStringList>
#include <QString>

namespace CDI
{
	// This class will only handle file addittions
	class FileSystemWatcher : public QFileSystemWatcher
	{
		Q_OBJECT
	public:
		FileSystemWatcher(QObject *parent = NULL);

		void setDirectory(QString &dirname);

	protected:
		QStringList _fileList;
		QString _watchedDir;

	public slots:
		void onInternalDirectoryChange(const QString& dir);

	signals:
		void fileAdded(const QString& newfile);
	};
}
