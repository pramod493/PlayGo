#ifndef FORCEITEM_H
#define FORCEITEM_H
#include "commonfunctions.h"
#include "abstractmodelitem.h"
#include <QGraphicsLineItem>
#include <QPen>
#include <QBrush>
#include <QLineF>
#include <QPainter>

namespace CDI
{
	class Component;
	class ForceGraphicsItem : public QGraphicsLineItem
	{
	protected:
		float _edgewidth;
		float _edgeangle;
	public:
		ForceGraphicsItem(QGraphicsItem* parent = 0)
			:QGraphicsLineItem(parent)
		{
			init();
		}

		ForceGraphicsItem(float x1, float y1, float x2, float y2, QGraphicsItem* parent = 0)
			:QGraphicsLineItem(x1,y1,x2,y2,parent)
		{
			init();
		}

		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0)
		{
			QLineF arrowLine = line();
			float angle = arrowLine.angle();
			painter->setPen(pen());
			painter->drawLine(arrowLine);
			arrowLine.setLength(_edgewidth);
			arrowLine.setAngle(angle - _edgeangle);
			painter->drawLine(arrowLine);
			arrowLine.setAngle(angle + _edgeangle);
			painter->drawLine(arrowLine);
		}

		QRectF boundingRect() const
		{
			QRectF base = QGraphicsLineItem::boundingRect();

			QPointF center = line().p1();
			QRectF local = QRectF(center.x()-_edgewidth, center.y()-_edgewidth,2*_edgewidth, 2*_edgewidth);
			return base.united(local);
		}

	protected:
		void init()
		{
			QPen _pen = QPen();
			_pen.setColor(Qt::red);
			_pen.setWidthF(3);
			setPen(_pen);

			_edgewidth = 20;
			_edgeangle = 30;
		}
	};

	class ForceItem : public Item
	{
	protected:
		QUuid _id;
		Component* _component;

	public:
		ForceItem();

		ItemType type() const { return FORCEITEM; }

		QUuid id() const { return _id; }

		QDataStream& serialize(QDataStream &stream) const;
		QDataStream& deserialize(QDataStream &stream);
	};
}
#endif // FORCEITEM_H
