#include "cdigraphicspathitem.h"
namespace CDI
{
    GraphicsPathItem::GraphicsPathItem(QGraphicsItem *parent, QPointF startPoint)
        : QGraphicsPathItem(parent)
    {
		painterPath = QPainterPath(startPoint);
        parentStroke = new Stroke(NULL);
		parentStroke->points.push_back(new Point2DPT(startPoint.x(), startPoint.y(),0,0));
		isOrphan = true;
		setPath(painterPath);

        QObject::connect(parentStroke, SIGNAL(ItemChanged(Stroke*)),
                this, SLOT(OnStrokeUpdate(Stroke*)));
    }

    GraphicsPathItem::GraphicsPathItem(QGraphicsItem *parent, Stroke *stroke)
      : QGraphicsPathItem(parent)
    {
        parentStroke = stroke;
        QObject::connect(parentStroke, SIGNAL(ItemChanged(Stroke*)),
                         this, SLOT(OnStrokeUpdate(Stroke*)));
        parentStroke->update();
    }

    GraphicsPathItem::~GraphicsPathItem()
    {
        if (parentStroke!= NULL) delete parentStroke;
    }

    void GraphicsPathItem::push_back(QPointF point)
    {
        parentStroke->points.push_back(new Point2DPT(point.x(), point.y(), 0,0));
        parentStroke->update();
    }

    void GraphicsPathItem::push_back(Point2DPT point)
    {
        parentStroke->points.push_back(new Point2DPT(point,point._pressure,point._time));
        parentStroke->update();
	}

    void GraphicsPathItem::ApplySmoothing(int order)
    {
        parentStroke->ApplySmoothing(order);
    }

	bool GraphicsPathItem::Selected(QPointF point, float extraWidth)
	{
		Point2D *p = new Point2D(point.x(),point.y());
		bool val = parentStroke->Selected(p, extraWidth);
		delete p;
		return val;
	}

    void GraphicsPathItem::OnStrokeUpdate(Stroke* stroke)
    {
        Q_UNUSED(stroke);
        painterPath = QPainterPath(QPointF(parentStroke->points[0]->x,parentStroke->points[0]->y));
        for (int i=1; i<parentStroke->points.size();i++)
            painterPath.lineTo(parentStroke->points[i]->x,parentStroke->points[i]->y);
        setPath(painterPath);
        update();
    }
}
