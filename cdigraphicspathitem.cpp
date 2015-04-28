#include "cdigraphicspathitem.h"
namespace CDI
{
	CDIGraphicsPathItem::CDIGraphicsPathItem(QGraphicsItem *parent, QPointF startPoint)
        : QGraphicsPathItem(parent)
    {
		painterPath = QPainterPath(startPoint);
		parentStroke = new Stroke();
		parentStroke->points.push_back(new Point2DPT(startPoint.x(), startPoint.y(),0,0));
		isOrphan = true;
		setPath(painterPath);
    }

    CDIGraphicsPathItem::~CDIGraphicsPathItem()
    {
        if (parentStroke!= NULL) delete parentStroke;
    }

	void CDIGraphicsPathItem::push_back(QPointF point)
    {
        painterPath.lineTo(point);
        setPath(painterPath);
        parentStroke->points.push_back(new Point2DPT(point.x(), point.y(), 0,0));
		update();
    }

	void CDIGraphicsPathItem::push_back(Point2DPT point)
	{
        painterPath.lineTo(QPointF(point.x,point.y));
        setPath(painterPath);
        parentStroke->points.push_back(new Point2DPT(point,point._pressure,point._time));
        update();
	}

    void CDIGraphicsPathItem::ApplySmoothing(int order)
    {
        parentStroke->ApplySmoothing(order);
        painterPath = QPainterPath(QPointF(parentStroke->points[0]->x, parentStroke->points[0]->y));
        for (int i=1; i<parentStroke->points.size();i++)
            painterPath.lineTo(QPointF(parentStroke->points[i]->x, parentStroke->points[i]->y));
        setPath(painterPath);
        update();
    }
}
