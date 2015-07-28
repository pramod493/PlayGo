#include "scenebutton.h"

namespace CDI
{
	SceneButton::SceneButton(const QPixmap &pixmap, QString filePath, QGraphicsItem *parent)
		: QGraphicsWidget(parent), _pix(pixmap)
	{
		_filePath = filePath;
		setAcceptHoverEvents(true);
		setCacheMode(DeviceCoordinateCache);
		_pix = _pix.scaled(QSize(64,64), Qt::KeepAspectRatio);
//		setFlag(QGraphicsItem::ItemIsMovable);
		setFlag(QGraphicsItem::ItemIgnoresTransformations);
	}

	void SceneButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
	{
		bool down = option->state & QStyle::State_Sunken;
		QRectF r = boundingRect();
		QLinearGradient grad(r.topLeft(), r.bottomRight());

		grad.setColorAt(down ? 1 : 0, option->state & QStyle::State_MouseOver ? Qt::white : Qt::lightGray);
		grad.setColorAt(down ? 0 : 1, Qt::darkGray);
		painter->setPen(Qt::darkGray);
		painter->setBrush(grad);
		painter->drawEllipse(r);
		QLinearGradient grad2(r.topLeft(), r.bottomRight());
		grad2.setColorAt(down ? 1 : 0, Qt::darkGray);
		grad2.setColorAt(down ? 0 : 1, Qt::lightGray);
		painter->setPen(Qt::NoPen);
		painter->setBrush(grad2);
		if (down)
			painter->translate(2, 2);
		painter->drawEllipse(r.adjusted(5, 5, -5, -5));
		painter->drawPixmap(-_pix.width()/2, -_pix.height()/2, _pix.width(), _pix.height(), _pix);
	}
}
