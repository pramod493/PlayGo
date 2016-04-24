#include "fileio.h"

namespace CDI
{
	bool FileIO::saveTo(QString filepath)
	{
		QFile f(filepath);
		if (f.open(QIODevice::WriteOnly) == false)
		{
			// Unable to open file in write only mode
			return false;
		}
		QDataStream stream(&f);
		saveTo(stream);
		f.close();
		return true;
	}

	bool FileIO::saveTo(QDataStream& stream)
	{
		Q_UNUSED(stream);
		// TODO - Implement this one to parse from top level
		return true;
	}

	bool FileIO::open(QString filepath, bool merge)
	{
		QFile f(filepath);
		if (f.exists() == false)
		{
			// File does not exist
			return false;
		}
		if (f.open(QIODevice::ReadOnly) == false)
		{
			// Unable to open ile in read only mode
			return false;
		}

		if (!merge) {
			// Clear memory
		}
		QDataStream stream(&f);
		open(stream);
		f.close();
		return true;
	}

	bool FileIO::open(QDataStream& stream)
	{
		Q_UNUSED(stream);
		// TODO - Implement
		return true;
	}
}
