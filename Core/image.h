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
	class Component;

	/* TODO
	 * The Image itself is not enough and we need to add metadata information
	 * to it as well
	 * */

	/**
     * @brief Represents Image information for display, triangulation,
     * and metadata container.
	 * @remarks: QRectF class need not be public since we are
	 * not supposed to change any of its attributes from outside
	 * Its dimensions must be set based on image dimensions ONLY.
	 */
    class Image : public AbstractModelItem
	{
	protected:
        QPixmap* _pixmap;
		QString _filepath;
		QTransform _transform;
		QTransform _inverseTransform;

	public:
		//-----------------------------------------------
		// Constructors/Destructors
		//-----------------------------------------------
		Image(Component* component);
		Image(Component* component, const QString filename);
		Image(const Image & image);
		Image(Component* component, const QPixmap & pixmap, QString filename);

        virtual ~Image();

		//-----------------------------------------------
		// Query/Set functions(same order in derived class)
		// Non-virtual
		//-----------------------------------------------
        QPixmap* pixmap() const;
		QString filepath() const;

        void setPixmap(QPixmap* pixmap);
        void setFilepath(QString filepath, bool updateImage = true);

		//-----------------------------------------------
		// Virtual functions (same order in derived class)
		//-----------------------------------------------
		virtual QRectF boundingRect() const;

		ItemType type() const;

		QTransform transform() const;

		void setTransform(QTransform transform);

		QTransform inverseTransform() const;

		QDataStream& serialize(QDataStream& stream) const;
		QDataStream& deserialize(QDataStream& stream);
	};

	/************************************************************
	 *  QImage inline functions
	 **********************************************************/
}
