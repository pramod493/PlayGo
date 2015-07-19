#include "penstroke.h"
namespace CDI
{
	PenStroke::PenStroke(QGraphicsItem *parent)
		: QGraphicsPathItem(parent)
	{
		points.clear();
		painterpath = QPainterPath();
	}

	PenStroke::~PenStroke()
	{
		points.clear();
	}

	void PenStroke::push_point(QPointF &point)
	{
		if (points.size())
		{
			painterpath.lineTo(point);
			points.push_back(point);
		} else {
			painterpath = QPainterPath(point);
			points.push_back(point);
		}
		setPath(painterpath);
	}
}
