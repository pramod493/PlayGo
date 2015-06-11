#include "image.h"
#include <QDebug>
namespace CDI
{
	Image::Image()
			: _filepath(""), _pixmap(), QRectF()
	{
	}

	Image::Image(const QString filename)
		: _filepath(filename)
	{
		QFile f(filename);
		_pixmap = QPixmap();
		if (f.exists())
		{
			_pixmap.load(f.fileName());
		}
	}

	ItemType Image::type() const
	{
		return ItemType::IMAGE;
	}

	QRectF Image::boundingRect() const
	{
		return _transform.mapRect(*this);
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

	void Image::updateRect()
	{
		int *x, *y, *width, *height;
		QRect box = _pixmap.rect();
		box.getCoords(x,y,width,height);
		setCoords(*x, *y, *width, *height);
	}
}
