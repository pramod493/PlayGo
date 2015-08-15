#include "SelectableActions.h"
#include "QsLog.h"
#include <QGraphicsSceneMouseEvent>
#include <QTouchEvent>
#include <QMessageBox>
namespace CDI
{
  // Initialize the static variable. This defines the size of all the
  int SelectableActions::dim = 48;

  SelectableActions::SelectableActions(QAction *action, QGraphicsItem *parent)
		:QGraphicsEllipseItem(parent)
	{
		_action = action;
		_text = _action->text();
		QIcon tmpIcon = _action->icon();
		int _dim = SelectableActions::dim;
		float ellipseDim = _dim*1.45f;
		QPixmap tmpPixmap = tmpIcon.pixmap(QSize(_dim,_dim));
		setRect(-ellipseDim/2, -ellipseDim/2, ellipseDim, ellipseDim);
		QRadialGradient radialGradient = QRadialGradient(QPointF(0,0), ellipseDim/2.0f);
		radialGradient.setColorAt(0, Qt::white);
		radialGradient.setColorAt(0.80f, QColor(255,255,255,200));
		radialGradient.setColorAt(0.99f, QColor(255,255,255,0));
		radialGradient.setSpread(QGradient::PadSpread);
		setFlag(QGraphicsItem::ItemIgnoresTransformations);
		setBrush(QBrush(radialGradient));
		setPen(QPen(Qt::NoPen));

		QGraphicsPixmapItem* pixmapItem = new QGraphicsPixmapItem(tmpPixmap, this);
		pixmapItem->setPos(-_dim/2,-_dim/2);
		pixmapItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		QGraphicsSimpleTextItem* textItem = new QGraphicsSimpleTextItem(_action->text(), this);
		textItem->setPos(-_dim/2, _dim/2);
		textItem->setScale(1.5f);
		textItem->setBrush(QBrush(Qt::red));
		//textItem->setPen(QPen(Qt::blue));

		//QMessageBox::about(0, "Enable selection", "Enable selection");
		setAcceptTouchEvents(true);
	}

	int SelectableActions::type() const
	{
		return Type;
	}

	void SelectableActions::trigger()
	{
		if (_action)
		{
			//QLOG_INFO() << _action->text() << "action selected from radial menu";
			_action->trigger();
		}
	}

	bool SelectableActions::sceneEvent(QEvent *event)
	{
		switch (event->type())
		{
		case QEvent::GraphicsSceneMousePress :
		case QEvent::TouchBegin :
		{
			event->accept();
			return true;
		}
		case QEvent::GraphicsSceneMouseRelease :
		{
			QGraphicsSceneMouseEvent* mouseEvent =
					static_cast<QGraphicsSceneMouseEvent*>(event);
			QPointF scenePos = mouseEvent->scenePos();
			if (contains(mapFromScene(scenePos)))
			{
				trigger();
				return true;
			}
			break;
		}
		case QEvent::TouchEnd :
		{
			// Accept only when the touch point still lies on the object
			QTouchEvent *touchEvent = static_cast<QTouchEvent*>(event);
			QList<QTouchEvent::TouchPoint> touchpoints = touchEvent->touchPoints();
			//foreach(QTouchEvent::TouchPoint tp, touchpoints)
			for(auto tp : touchpoints)
			{
				QPointF scenePos = tp.scenePos();
				if (contains(mapFromScene(scenePos)))
				{
					trigger();
					return true;
				}
			}
			break;
		}
		}
		return QGraphicsEllipseItem::sceneEvent(event);
	}
}
