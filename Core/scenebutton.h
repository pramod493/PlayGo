#ifndef SCENEBUTTON_H
#define SCENEBUTTON_H
#include <QGraphicsWidget>
#include <QDebug>
#include <QPixmap>
#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include "QsLog.h"
namespace CDI
{
	class SceneButton : public QGraphicsWidget
	{
		Q_OBJECT
	public:
		SceneButton(const QPixmap &pixmap, QString filePath, QGraphicsItem *parent = 0);

		QRectF boundingRect() const
		{
			return QRectF(-65, -65, 130, 130);
		}

		QPainterPath shape() const
		{
			QPainterPath path;
			path.addEllipse(boundingRect());
			return path;
		}

		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *);

	signals:
		void pressed(QString filepath);

	protected:
		void mousePressEvent(QGraphicsSceneMouseEvent *)
		{
			QLOG_INFO() << _filePath << "clicked...";
		}

		void mouseReleaseEvent(QGraphicsSceneMouseEvent *)
		{
			emit pressed(_filePath);
		}

	private:
		QPixmap _pix;
		QString _filePath;
	};

}
#endif // SCENEBUTTON_H
