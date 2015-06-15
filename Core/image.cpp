#include "image.h"
#include "component.h"
#include <QDebug>
namespace CDI
{
	Image::Image(Component* component)
			: _filepath(""), _pixmap(), QRectF()
	{
		setParentItem(component);
	}

	Image::Image(Component* component, const QString filename)
		: _filepath(filename)
	{
		setParentItem(component);

		QFile f(filename);
		_pixmap = QPixmap();
		if (f.exists()) _pixmap.load(f.fileName());
		updateRect();
	}

	Image::Image(const Image & image)
		: _pixmap(image.pixmap()), _filepath("")
	{
		setParentItem(image.parentItem());
		updateRect();
	}

	Image::Image(Component* component, const QPixmap &pixmap, QString filename)
		: _pixmap(pixmap), _filepath(filename)
	{
		setParentItem(component);
		updateRect();
	}

	QPixmap Image::pixmap() const
	{
		return _pixmap;
	}

	QString Image::filepath() const
	{
		return _filepath;
	}

	void Image::setPixmap(QPixmap& pixmap)
	{
		_pixmap = pixmap;
		mask |= isModified;
		updateRect();
	}

	void Image::setFilepath(QString filepath)
	{
		_filepath = filepath;
		mask |= isModified;
	}

	QRectF Image::boundingRect() const
	{
		return _transform.mapRect(*this);
	}

	ItemType Image::type() const
	{
		return ItemType::IMAGE;
	}

	QTransform Image::transform() const
	{
		return _transform;
	}

	void Image::setTransform(QTransform transform)
	{
		_transform = transform;
		mask |= isTransformed;
		_inverseTransform = transform.inverted();
	}

	QTransform Image::inverseTransform() const
	{
		return _inverseTransform;
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
