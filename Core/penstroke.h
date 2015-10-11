#ifndef PENSTROKE_H
#define PENSTROKE_H
#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QVector>
#include <QVectorIterator>
#include "point2dpt.h"
#include "commonfunctions.h"
#include "cdi2qtwrapper.h"

namespace CDI
{
	class PenStroke : public QGraphicsPathItem
	{
protected:
		QPainterPath painterpath;

	public:
		enum { Type = UserType + PENSTROKEVIEW };

		QVector<QPointF> points;

		PenStroke(QGraphicsItem* parent = 0);

		virtual ~PenStroke();

		int type() const { return Type; }

		void push_point(QPointF& point);
	};
}
#endif // PENSTROKE_H
