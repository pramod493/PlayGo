#include "SelectableActions.h"
#include "QsLog.h"

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

    void SelectableActions::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
    {
        mouseEvent->accept();
    }

    void SelectableActions::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
    {

    }

    void SelectableActions::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
    {
        mouseEvent->accept();
    }

    bool SelectableActions::sceneEvent(QEvent *event)
    {
        switch (event->type())
        {
            case QEvent::TouchBegin :
            {
                event->accept();
                return true;
            }
            case QEvent::TouchEnd :
            {
                // Accept only when the touch point still lies on the object
                
            }
        }
		return QGraphicsEllipseItem::sceneEvent(event);
    }
}
