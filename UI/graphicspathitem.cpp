#include "graphicspathitem.h"
#include <QPainter>
#include <QPainterPath>
#include <QVectorIterator>
#include <QDebug>
#include "graphicsitemgroup.h"

using namespace std;

namespace CDI
{

	// TODO - Add stroke and thickness information to the stroke
//	GraphicsPathItem::GraphicsPathItem(QGraphicsItem *parent, Point2D startPoint, float pressure, int time)
//		: QGraphicsPathItem (parent)
//	{
//		parentStroke = new Stroke();
//		parentStroke->setTransform
//				(parentStroke->transform().translate(startPoint.x(), startPoint.y()));
//		setTransform(parentStroke->transform());
//		push_back(startPoint,pressure,time);
//    }

	GraphicsPathItem::GraphicsPathItem(GraphicsItemGroup *parent, Stroke *stroke)
		: QGraphicsPathItem (parent)
    {
		parentStroke = stroke;
		update();
    }

    GraphicsPathItem::~GraphicsPathItem()
    {
		// Do not delete the parentStroke object here. This is deleted when parentStroke object is deleted
    }

    void GraphicsPathItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
		Q_UNUSED(widget)
		Q_UNUSED(option)
		// Hide when hidden
		if (parentStroke->isVisible() == false) return;
		if (parentStroke->size() < 3) return;

		QPen pen = this->pen();
		pen.setColor(parentStroke->color());
		float width = parentStroke->thickness();

		Point2DPT* data = parentStroke->data();		// gives out the data. Careful not to overwrite that information
		int num_points = parentStroke->size();

		for (int i=1; i< num_points; i++)
		{
			Point2DPT p1 = data[i-1]; Point2DPT p2 = data[i];
			pen.setWidthF(width * p1.pressure()); painter->setPen(pen);
			painter->drawLine(p1,p2);
		}
    }

	void GraphicsPathItem::push_back(QPointF point, float pressure, int time)
    {
		if (parentStroke!= NULL)
		{
			Point2D pt = mapFromScene(point);
			parentStroke->push_point(Point2DPT(pt.x(),pt.y(), pressure, time));
		}
    }

    void GraphicsPathItem::push_back(Point2DPT point)
    {
		if (parentStroke!= NULL)
		{
			Point2D pt = mapFromScene(point);
			point.setX(pt.x()); point.setY(pt.y());
			parentStroke->push_point(point);
		}
    }

    void GraphicsPathItem::ApplySmoothing(int order)
    {
		if (parentStroke!= NULL) parentStroke->applySmoothing(order);
    }

    bool GraphicsPathItem::Selected(QPointF point, float extraWidth)
    {
		if (parentStroke== NULL) return false;
		Point2D p = Point2D(point.x(),point.y());
		bool val = parentStroke->containsPoint(p, SelectionType::OnItem, extraWidth);
        return val;
    }

	QRectF GraphicsPathItem::boundingRect() const
	{
		if (parentStroke== NULL) return QRectF();
		return parentStroke->boundingRect();
	}

	void GraphicsPathItem::updateStroke()
    {
		if (parentStroke!= NULL)
			setTransform(parentStroke->transform());
    }
}
