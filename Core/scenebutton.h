#ifndef SCENEBUTTON_H
#define SCENEBUTTON_H
#include <QGraphicsWidget>
#include <QPixmap>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>

namespace CDI
{
	class SceneButton : public QGraphicsWidget
	{
		Q_OBJECT
	public:
		SceneButton(const QPixmap &pixmap, QString filePath, QGraphicsItem *parent = 0);

		QRectF boundingRect() const Q_DECL_OVERRIDE
		{
			return QRectF(_pix.rect());
		}

		QPainterPath shape() const Q_DECL_OVERRIDE
		{
			QPainterPath path;
			path.addEllipse(boundingRect());
			return path;
		}

		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);

	signals:
		void pressed(QString filepath);

	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent *) Q_DECL_OVERRIDE
		{
			//        emit pressed();
			//        update();
		}

		void mouseReleaseEvent(QGraphicsSceneMouseEvent *) Q_DECL_OVERRIDE
		{
			//        update(_filePath);
		}

	private:
		QPixmap _pix;
		QString _filePath;
	};

}
#endif // SCENEBUTTON_H
