#pragma once
#include <QGraphicsPixmapItem>
#include "cdi2qtwrapper.h"
#include "commonfunctions.h"
#include <QFile>
#include "physicsshape.h"

namespace CDI
{
	class Pixmap : public QGraphicsPixmapItem
	{
	public:
        enum {Type = UserType + IMAGEVIEW };

    protected:
        bool _highlighted;
		QUuid _id;
		QString _filename;

		PhysicsShape* _physicsShape;

    public:
		Pixmap(QGraphicsItem* parent = 0);

		Pixmap(QString filepath, QGraphicsItem* parent = 0);

		Pixmap(const QPixmap &pixmap, QString filepath, QGraphicsItem* parent = 0);

		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

		QRectF boundingRect() const {
			if (isVisible()== false) return QRectF();
			return QGraphicsPixmapItem::boundingRect();
		}

		PhysicsShape* physicsShape() const;

		void resetPhysicsShape();

		int type() const { return Type; }

		QUuid id() const { return _id; }

		QString filename() const;

		void setFilename(QString filepath);

        bool isHighlighted() const;

        void highlight(bool value);

		QDataStream& serialize(QDataStream& stream) const;

		QDataStream& deserialize(QDataStream& stream);

	protected:
		void initializePhysicsShape();
	};
}
