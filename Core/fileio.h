#pragma once
#include <QDataStream>
#include <QFile>
#include <QDir>
#include <QVector>
#include <QHash>
#include "commonfunctions.h"
#include "abstractmodelitem.h"
#include "stroke.h"
#include "image.h"
#include "component.h"
#include "playgo.h"

namespace CDI
{
	/**
	 * @brief The FileIO class is a top level class and manages the IO operations
	 * PlayGo core datastructure. Also, since it is top level class, it is aware
	 * of all the classes
	 */
	class FileIO
	{
	protected:
		QFile _fname;

		bool _overwrite;

	public:
		inline FileIO();

		inline QString currentFile() const;

		inline bool overwrite();

		inline void setFile(QFile &filepath);

		inline void overwriteEnable(bool enable);

		/**
		 * @brief SaveTo: Save everything to default file path
		 * @return True, if success, false otherwise
		 */
		inline bool saveTo();

		/**
		 * @brief SaveTo: Save everything to given file name
		 * @return True, if success, false otherwise
		 */
		bool saveTo(QString file);

		/**
		 * @brief SaveTo: Save to given QDataStream
		 * @param stream: Input QDataStream
		 */
		bool saveTo(QDataStream& stream);

		/**
		 * @brief Open: Reads given file and creates new datastructure
		 * Any conflicting information will be ignored
		 * @param filepath: File to read
		 * @param merge: Will merge the information loaded in file and in memory, if true. If false, will clear memory before loaing
		 * @return True, if success, false otherwise
		 */
		bool open(QString filepath, bool merge = false);

		/**
		 * @brief Open: Reads from given stream and creates new datastructure
		 * Any conflicting information will be ignored
		 * @param file: QDataStream for reading
		 * @return Tue, if success, false otherwise
		 */
		bool open(QDataStream& stream);

		/**
		 * @brief merge: Reads given file and appends information to current one
		 * Any conflicting information will be ignored
		 * @param file: File to read
		 * @return True, if success, false otherwise
		 */
		inline bool merge(QString file);

	};

	inline FileIO::FileIO()
		:_fname("PlayGo.dat")
	{
	}

	inline QString FileIO::currentFile() const
	{
		return _fname.fileName();
	}

	inline bool FileIO::overwrite()
	{
		return _overwrite;
	}

	inline void FileIO::setFile(QFile &filepath)
	{
		_fname.setFileName(filepath.fileName());
	}

	inline void FileIO::overwriteEnable(bool enable)
	{
		_overwrite = enable;
	}

	inline bool FileIO::saveTo()
	{
		return saveTo(_fname.fileName());
	}

	inline bool FileIO::merge(QString file)
	{
		return open(file, true);
	}
}
