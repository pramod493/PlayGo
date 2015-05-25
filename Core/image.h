#pragma once
#include <QDataStream>
#include <QPixmap>
#include <QString>
#include <QTransform>
#include <QFile>
#include "commonfunctions.h"
#include "abstractmodelitem.h"

namespace CDI
{
	class Image : public AbstractModelItem
	{
	protected:
		QPixmap _pixmap;
		QString _filepath;
		QTransform _transform;

	public:
		inline Image();
		Image(const QString filename);
		inline Image(const Image & image);
		inline Image(const QPixmap & pixmap, QString filename);

		inline QPixmap pixmap() const;
		inline QString filepath() const;
		inline QTransform transform() const;

		inline void setPixmap(QPixmap& pixmap);
		inline void setFilepath(QString filepath);
		inline void setTransform(QTransform& transform);

		// Virtual functions
		virtual ItemType type() const;
		QDataStream& serialize(QDataStream& stream) const;
		QDataStream& deserialize(QDataStream& stream);
	};

	/************************************************************
	 *  QImage inline functions
	 **********************************************************/
	inline Image::Image()
		: _filepath(""), _pixmap()
	{
	}

	inline Image::Image(const Image & image)
		: _pixmap(image.pixmap()), _filepath("")
	{}

	inline Image::Image(const QPixmap &pixmap, QString filename)
		: _pixmap(pixmap), _filepath(filename)
	{}

	inline QPixmap Image::pixmap() const
	{
		return _pixmap;
	}

	inline QString Image::filepath() const
	{
		return _filepath;
	}

	inline QTransform Image::transform() const
	{
		return _transform;
	}

	inline void Image::setPixmap(QPixmap& pixmap)
	{
		_pixmap = pixmap;
		mask |= isModified;
	}

	inline void Image::setFilepath(QString filepath)
	{
		_filepath = filepath;
		mask |= isModified;
	}

	inline void Image::setTransform(QTransform& transform)
	{
		_transform = transform;
		mask |= isTransformed;
	}
}
