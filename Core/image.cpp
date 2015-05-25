#include "image.h"
#include <QDebug>
namespace CDI
{
	Image::Image(const QString filename)
		: _filepath(filename)
	{
		QFile f(filename);
		_pixmap = QPixmap();
		if (f.exists())
			_pixmap.load(f.fileName());
	}

	ItemType Image::type() const
	{
		return ItemType::IMAGE;
	}

	QDataStream& Image::serialize(QDataStream & stream) const
	{
		stream << filepath();
		stream << transform();
		stream << pixmap();
		return stream;
	}

	QDataStream & Image::deserialize(QDataStream & stream)
	{
		QString filepath;
		QTransform transform;
		QPixmap pixmap;

		stream >> filepath;
		stream >> transform;
		stream >> pixmap;

		setFilepath(filepath);
		setTransform(transform);
		setPixmap(pixmap);

		return stream;
	}
}
