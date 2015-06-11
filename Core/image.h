#pragma once
#include <QDataStream>
#include <QPixmap>
#include <QString>
#include <QTransform>
#include <QFile>
#include "commonfunctions.h"
#include "abstractmodelitem.h"
#include <QRectF>

namespace CDI
{
	/**
	 * @brief The Image class
	 * @remarks: QRectF class need not be public since we are
	 * not supposed to change any of its attributes from outside
	 * Its dimensions must be set based on image dimensions ONLY.
	 */
	class Image : public AbstractModelItem, protected QRectF
	{
		Q_OBJECT
		Q_PROPERTY(QPixmap image READ pixmap WRITE setPixmap NOTIFY pixmapChanged)
		Q_PROPERTY(QString filepath READ filepath WRITE setFilepath NOTIFY filepathChanged)
		Q_PROPERTY(QTransform transform READ transform WRITE setTransform)
		Q_PROPERTY(QTransform inverseTransform READ inverseTransform)
	protected:
		QPixmap _pixmap;
		QString _filepath;
		QTransform _transform;
		QTransform _inverseTransform;

	public:
		Image();
		Image(const QString filename);
		inline Image(const Image & image);
		inline Image(const QPixmap & pixmap, QString filename);

		inline QPixmap pixmap() const;
		inline QString filepath() const;
		inline QTransform transform() const;
		inline QTransform inverseTransform() const;
		virtual QRectF boundingRect() const;

		inline void setPixmap(QPixmap& pixmap);
		inline void setFilepath(QString filepath);
		inline void setTransform(QTransform& transform);

		// Virtual functions
		virtual ItemType type() const;
		QDataStream& serialize(QDataStream& stream) const;
		QDataStream& deserialize(QDataStream& stream);
	protected:
		void updateRect();

	signals:
		void pixmapChanged(Image* image);
		void filepathChanged(Image* image);
	};

	/************************************************************
	 *  QImage inline functions
	 **********************************************************/
	inline Image::Image(const Image & image)
		: _pixmap(image.pixmap()), _filepath("")
	{
		updateRect();
	}

	inline Image::Image(const QPixmap &pixmap, QString filename)
		: _pixmap(pixmap), _filepath(filename)
	{
		updateRect();
	}

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

	inline QTransform Image::inverseTransform() const
	{
		return _inverseTransform;
	}

	inline void Image::setPixmap(QPixmap& pixmap)
	{
		_pixmap = pixmap;
		mask |= isModified;
		updateRect();
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
		_inverseTransform = transform.inverted();
	}
}

Q_DECLARE_METATYPE(CDI::Image)
