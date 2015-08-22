#include "forceitem.h"

namespace CDI
{
	//Set the scale for force graphics item
	float ForceGraphicsItem::forceScale = 1.0f;

	ForceGraphicsItem::ForceGraphicsItem(QGraphicsItem *parent)
		:QGraphicsPolygonItem(parent)
	{
		setFlag(QGraphicsItem::ItemIgnoresTransformations);
		setFlag(QGraphicsItem::ItemIgnoresParentOpacity);

		_id = uniqueHash();

		QPen _pen = QPen();
		_pen.setColor(Qt::red);
		_pen.setWidthF(3);
		setPen(_pen);
	}

	ForceGraphicsItem::ForceGraphicsItem(QPointF forceposition, QPointF forcevector, QGraphicsItem *parent)
		:ForceGraphicsItem(parent)
	{
		_forcePosition = forceposition;
		_forceVector = forcevector;

		initialize();
	}

	void ForceGraphicsItem::setLine(float x0, float y0, float fx, float fy)
	{
		_forcePosition.setX(x0);
		_forcePosition.setY(y0);

		_forceVector.setX(fx);
		_forceVector.setY(fy);

		initialize();
	}

	QPointF ForceGraphicsItem::forcePosition() const
	{
		return _forcePosition;
	}

	void ForceGraphicsItem::setForcePosition(QPointF pos)
	{
		_forcePosition = pos;

		initialize();
	}

	QPointF ForceGraphicsItem::forceVector() const
	{
		return _forceVector;
	}

	void ForceGraphicsItem::setForceVector(QPointF pos)
	{
		_forceVector = pos;

		initialize();
	}

	void ForceGraphicsItem::initialize()
	{
		thickness_main = 6;
		thickness_head = 14;
		length_head = 10;

		float vectorLength = magnitude(&_forceVector) * forceScale;
		QVector<QPointF> points;

		points.push_back(QPointF(0,0));
		points.push_back(QPointF(0, thickness_main));
		points.push_back(QPointF(vectorLength, thickness_main));
		points.push_back(QPointF(vectorLength, thickness_head));
		points.push_back(QPointF(vectorLength+length_head, 0));
		points.push_back(QPointF(vectorLength, -thickness_head));
		points.push_back(QPointF(vectorLength, -thickness_main));
		points.push_back(QPointF(0, -thickness_main));

		setPolygon(QPolygonF(points));

		setPos(_forcePosition);
		auto vectorangle = angleWithX(&_forceVector) * TO_DEGREES_FROM_RAD;
		setRotation(vectorangle);
	}

	QDataStream& ForceGraphicsItem::serialize(QDataStream &stream) const
	{
		stream << _id;
		stream << _forcePosition;
		stream << _forceVector;
		stream << thickness_main;
		stream << thickness_head;
		stream << length_head;

		// No need to save transform of this
		return stream;
	}

	QDataStream& ForceGraphicsItem::deserialize(QDataStream &stream)
	{
		stream >> _id;
		stream >> _forcePosition;
		stream >> _forceVector;
		stream >> thickness_main;
		stream >> thickness_head;
		stream >> length_head;

		initialize();
		return stream;
	}
}
