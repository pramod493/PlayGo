#include "image.h"
#include "component.h"
#include <QDebug>
#include "QsLog.h"
namespace CDI
{
	Image::Image(Component* component)
            : _filepath("")
	{
        _pixmap = NULL;
		setParentItem(component);
	}

	Image::Image(Component* component, const QString filename)
		: _filepath(filename)
	{
		setParentItem(component);

        QFile f(filename);
        _pixmap = new QPixmap();
        if (f.exists()) _pixmap->load(f.fileName());
	}

	Image::Image(const Image & image)
        : _filepath("")
	{
		setParentItem(image.parentItem());
        _filepath = image.filepath();
        _pixmap = image.pixmap();   // Do not copy the image. Rather share the pointer
	}

	Image::Image(Component* component, const QPixmap &pixmap, QString filename)
        : _filepath(filename)
	{
		setParentItem(component);
        _pixmap = new QPixmap(pixmap);
	}

    Image::~Image()
    {
        if (_pixmap != NULL)
            delete _pixmap;

		if (parentItem() != NULL)
			parentItem()->removeItem(id());
    }

    QPixmap *Image::pixmap() const
	{
		return _pixmap;
	}

	QString Image::filepath() const
	{
		return _filepath;
	}

    void Image::setPixmap(QPixmap *pixmap)
	{
        if (pixmap!= NULL) delete _pixmap;
		_pixmap = pixmap;
		mask |= isModified;

	}

    void Image::setFilepath(QString filepath, bool updateImage)
	{
		_filepath = filepath;
        if (updateImage)
        {
            if (_pixmap!= NULL) delete _pixmap;
            QFile f(_filepath);
            if (f.exists())
                _pixmap->load(f.fileName());
        }
        mask |= isModified;
	}

	QRectF Image::boundingRect() const
	{
        if (_pixmap != NULL)
            return QRectF(_pixmap->rect());
        return QRectF();
	}

	ItemType Image::type() const
	{
		return IMAGE;
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
        stream << *_pixmap;
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
        _pixmap = new QPixmap(pixmap);

		return stream;
	}
}
