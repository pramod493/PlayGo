#include "pinjointlimitselector.h"
namespace CDI
{
	class CustomArc : public QGraphicsPathItem
	{
	public :
		QPointF center;
		int radius;
		int startAngle;
		int endAngle;
		QPen arcPen;
		QBrush arcBrush;

		CustomArc(QGraphicsItem *parent)
			: QGraphicsPathItem(parent)
		{
			center = QPointF();
			radius = 0;
			startAngle = 0;
			endAngle = 0;
			arcPen = QPen(Qt::black);
			arcPen.setWidth(3);
			arcBrush = QBrush(Qt::NoBrush);
		}

		QRectF boundingRect() const
		{
			return QRectF(center.x()-radius, center.y()-radius, 2*radius, 2*radius);
		}

	protected:
		void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
		{
			Q_UNUSED(option)
			Q_UNUSED(widget)
			return;
			painter->drawArc(boundingRect(), startAngle*16, (endAngle-startAngle)*16);
			return;
			painter->setPen(arcPen);
			painter->setBrush(arcBrush);
			QPointF startPos = QPointF(radius*sin(startAngle*TO_RADIANS_FROM_DEG),
									   radius*cos(startAngle*TO_RADIANS_FROM_DEG));
			for (int i = startAngle; i < endAngle; i+=3)
			{
				QPointF nextPos = QPointF(radius*sin(i*TO_RADIANS_FROM_DEG),
										   radius*cos(i*TO_RADIANS_FROM_DEG));
				painter->drawLine(startPos, nextPos);
				startPos = nextPos;
			}

		}
	};

	RangeDialHandle::RangeDialHandle(int handleLength, int handleRadius, QGraphicsItem *graphicsparent)
		: QObject(NULL), QGraphicsPathItem(graphicsparent)
	{
		// No need to set the pen info here. We will do outside
		QPainterPath painterPath;
		painterPath.moveTo(0,0);
		painterPath.lineTo(handleLength, 0);
		painterPath.addEllipse(handleLength, -handleRadius, 2*handleRadius, 2*handleRadius);
		setPath(painterPath);

		// Add extra ring
		QPainterPath tmp; tmp.moveTo(0,0);
		float offset = 0.30f;
		tmp.addEllipse(handleLength-handleRadius*offset, -handleRadius*(1+offset), 2*handleRadius*(1+offset), 2*handleRadius*(1+offset));
		QGraphicsPathItem *extraOutline = new QGraphicsPathItem(tmp, this);
		QPen pen = QPen(QColor(50,0,0)); pen.setWidth(3);
		extraOutline->setPen(pen);

		setAcceptTouchEvents(true);
		setFlag(QGraphicsItem::ItemIgnoresTransformations);

		_angle = 0;
		_tmpAngle = 0;
		_itemIsLocked = false;
		_startPos = QPointF();
		_prevPos = QPointF();

		textItem = new QGraphicsSimpleTextItem(this);
		textItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
		textItem->setPos (handleLength + (2.25f+ offset)*handleRadius, 0);
		textItem->setText(QString("0°"));
		textItem->setScale(1.50f);
		textItem->setBrush(QBrush(QColor(50,0,0)));
	}

	int RangeDialHandle::currentAngle()
	{
		return static_cast<int>(_tmpAngle);
	}

	bool RangeDialHandle::sceneEvent(QEvent* event)
	{
		switch (event->type())
		{
		case QEvent::GraphicsSceneMousePress :
		{
			QGraphicsSceneMouseEvent* mouseEvent =
					static_cast<QGraphicsSceneMouseEvent*>(event);
			_startPos = mouseEvent->scenePos();
			_itemIsLocked = true;
			event->accept();
			return true;
		}
		case QEvent::GraphicsSceneMouseMove :
		{
			// TODO - Use ID so that we can manage this even when multi-fingers are involved
			if (!_itemIsLocked) return false;	// Item is not locked for some reason

			QGraphicsSceneMouseEvent* mouseEvent =
					static_cast<QGraphicsSceneMouseEvent*>(event);

			QPointF origin = mapToScene(0,0);
			QPointF currentPos = mouseEvent->scenePos() - origin;
			QPointF initialPos = _startPos - origin;

			float deltaAngle = (angleWithX(&currentPos) - angleWithX(&initialPos)) * 180.0f/_PI_;
			_tmpAngle = _angle + deltaAngle;
			setRotation(_angle + deltaAngle);

			emit signalAngleChanged();

			textItem->setText(QString::number(static_cast<int>(_angle+deltaAngle)) + QString("°"));
			event->accept();
			return true;
		}
		case QEvent::GraphicsSceneMouseRelease :
		{
			if (!_itemIsLocked) return false;	// Item is not locked for some reason

			QGraphicsSceneMouseEvent* mouseEvent =
					static_cast<QGraphicsSceneMouseEvent*>(event);

			QPointF origin = mapToScene(0,0);
			QPointF currentPos = mouseEvent->scenePos() - origin;
			QPointF initialPos = _startPos - origin;

			float deltaAngle = (angleWithX(&currentPos) - angleWithX(&initialPos)) * 180.0f/_PI_;
			if (abs(deltaAngle) > 1.0f) {
				_angle += static_cast<int>(deltaAngle);
				setRotation(_angle);
				emit signalAngleChanged();
			} else
			{
				setRotation(_angle);
			}
			_tmpAngle = _angle;

			emit signalAngleChanged();

			textItem->setText(QString::number(_angle) + QString("°"));
			event->accept();
			_itemIsLocked = false;
			return true;
		}
		case QEvent::TouchBegin :
		{
			QTouchEvent* touchEvent = static_cast<QTouchEvent*>(event);
			const QTouchEvent::TouchPoint &tp = touchEvent->touchPoints().first();
			_startPos = tp.scenePos();
			_itemIsLocked = true;
			event->accept();
			return true;
		}
		case QEvent::TouchUpdate :
		{
			// TODO - Use ID so that we can manage this even when multi-fingers are involved
			if (!_itemIsLocked) return false;	// Item is not locked for some reason

			QTouchEvent* touchEvent = static_cast<QTouchEvent*>(event);
			const QTouchEvent::TouchPoint &tp = touchEvent->touchPoints().first();
			QPointF origin = mapToScene(0,0);
			QPointF currentPos = tp.scenePos() - origin;
			QPointF initialPos = _startPos - origin;

			float deltaAngle = (angleWithX(&currentPos) - angleWithX(&initialPos)) * 180.0f/_PI_;
			_tmpAngle = _angle + deltaAngle;

			emit signalAngleChanged();

			setRotation(_angle + deltaAngle);
			textItem->setText(QString::number(static_cast<int>(_angle+deltaAngle)) + QString("°"));
			event->accept();
			return true;
		}
		case QEvent::TouchEnd :
		{
			if (!_itemIsLocked) return false;	// Item is not locked for some reason

			QTouchEvent* touchEvent = static_cast<QTouchEvent*>(event);
			const QTouchEvent::TouchPoint &tp = touchEvent->touchPoints().first();
			QPointF origin = mapToScene(0,0);
			QPointF currentPos = tp.scenePos() - origin;
			QPointF initialPos = _startPos - origin;

			float deltaAngle = (angleWithX(&currentPos) - angleWithX(&initialPos)) * 180.0f/_PI_;
			if (abs(deltaAngle) > 1.0f) {
				_angle += static_cast<int>(deltaAngle);
				setRotation(_angle);
			} else
			{
				setRotation(_angle);
			}
			_tmpAngle = _angle;

			emit signalAngleChanged();

			textItem->setText(QString::number(_angle) + QString("°"));
			event->accept();
			_itemIsLocked = false;
			return true;
		}
		case QEvent::TouchCancel :
		{
			_tmpAngle = _angle;
			setRotation(_angle);
			_itemIsLocked = false;
			event->accept();
			return true;	// NOTE - Do we need to accept event/return true in this case?
		}
		default :
			break;
		}
		// handle rest of the events in similat manner
		return QGraphicsPathItem::sceneEvent(event);
	}

	void RangeDialHandle::setText(QString text)
	{
		textItem->setText(text);
		//textItem->setText(QString::number(_angle) + QString("°"));
	}

	void RangeDialHandle::setAngle(int value)
	{
		if (_angle == value) return;
		_angle = value;
		_tmpAngle = _angle;
		setRotation(_angle);
		emit signalAngleChanged();
	}

	/*----------------------------------------------------------------------------
	 * -------------- Pin Joint Limits selection -------------------------------*/

	PinJointLimitsSelector::PinJointLimitsSelector
		(cdPinJoint *physicsJoint, QGraphicsItem *graphicsparent)
		: QObject (0), QGraphicsItemGroup(graphicsparent)
	{
		p_physicsJoint = physicsJoint;
		p_upperLimitHandle = new RangeDialHandle(175, 30, this);	// Keeps it under the next one
		p_lowerLimitHandle = new RangeDialHandle(100, 30, this);

		QPen pen = QPen();
		pen.setColor(Qt::black);
		pen.setWidth(5);

		p_lowerLimitHandle->setPen(pen);
		p_lowerLimitHandle->setBrush(QBrush(QColor(255,100,100)));

		p_upperLimitHandle->setPen(pen);
		p_upperLimitHandle->setBrush(QBrush(QColor(100,100,255)));

		p_lowerLimitHandle->setAngle(p_physicsJoint->lowerLimit());
		p_upperLimitHandle->setAngle(p_physicsJoint->upperLimit());

		p_connectorItem = new CustomArc(this);
		onAngleChanged();

		connect(p_lowerLimitHandle, SIGNAL(signalAngleChanged()),
				this, SLOT(onAngleChanged()));
		connect(p_upperLimitHandle, SIGNAL(signalAngleChanged()),
				this, SLOT(onAngleChanged()));

		// create handle for current angle
		auto currentAngleHandle = new RangeDialHandle(250, 15, this);
		currentAngleHandle->setAngle(p_physicsJoint->jointAngle());
		currentAngleHandle->setPen((pen.setWidth(2), pen));
		currentAngleHandle->setBrush(QBrush(Qt::yellow));
		currentAngleHandle->setAcceptTouchEvents(false);
		currentAngleHandle->setAcceptedMouseButtons(false);
		currentAngleHandle->setText(
					QString("Current angle=") +
					QString::number((int)p_physicsJoint->jointAngle())+ QString("°"));
	}

	PinJointLimitsSelector::~PinJointLimitsSelector()
	{

	}

	int PinJointLimitsSelector::getLowerLimitAngle() const
	{
		return lowerAngle;
	}

	int PinJointLimitsSelector::getUpperLimitAngle() const
	{
		return upperAngle;
	}

	void PinJointLimitsSelector::onAngleChanged()
	{
		lowerAngle = p_lowerLimitHandle->currentAngle();
		upperAngle = p_upperLimitHandle->currentAngle();

//		if (lowerAngle < 0) lowerAngle += 360;
//		if (upperAngle < 0) upperAngle += 360;
		// update the text
		p_lowerLimitHandle->setText(QString::number(lowerAngle) + QString("°"));
		p_upperLimitHandle->setText(QString::number(upperAngle) + QString("°"));

		p_connectorItem->radius = 70;
		p_connectorItem->startAngle = min(lowerAngle, upperAngle);
		p_connectorItem->endAngle = max(lowerAngle, upperAngle);
		p_connectorItem->scene()->update(p_connectorItem->mapRectToScene(p_connectorItem->boundingRect()));

		p_physicsJoint->enableLimits(true, min(lowerAngle, upperAngle),  max(lowerAngle, upperAngle));
	}
}

