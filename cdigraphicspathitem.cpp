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

//		parentStroke->points.push_back(new Point2DPT(point.x(),point.y(),0,0));
//		painterPath.lineTo(point);
		update();
    }

	void CDIGraphicsPathItem::push_back(Point2DPT point)
	{
		Q_UNUSED(point);
	}
}
