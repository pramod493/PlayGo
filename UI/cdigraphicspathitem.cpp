#include "cdigraphicspathitem.h"
#include <QPainter>
#include <QPainterPath>
#include <QVectorIterator>
#include <QDebug>

using namespace std;

namespace CDI
{
	// TODO - Add stroke and thickness information to the stroke
	GraphicsPathItem::GraphicsPathItem(QGraphicsItem *parent, Point2D startPoint, float pressure, int time)
		: QGraphicsPathItem (parent)
	{
		parentStroke = new Stroke();
		parentStroke->push_back(Point2DPT(startPoint.x(), startPoint.y(),pressure,time));
    }

    GraphicsPathItem::GraphicsPathItem(QGraphicsItem *parent, Stroke *stroke)
		: QGraphicsPathItem (parent)
    {
		parentStroke = stroke;
		update();
    }

    GraphicsPathItem::~GraphicsPathItem()
    {
        if (parentStroke!= NULL) delete parentStroke;
    }

    void GraphicsPathItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
		Q_UNUSED(widget)
		Q_UNUSED(option)
		if (parentStroke->size() < 3) return;

		QPen pen = this->pen();
		pen.setColor(parentStroke->color());
		float width = parentStroke->thickness();

		Point2DPT* data = parentStroke->data();		// gives out the data. Careful not to overwrite that information
		int num_points = parentStroke->size();
		painter->setTransform(parentStroke->transform());

		for (int i=1; i< num_points; i++)
		{
			Point2DPT p1 = data[i-1]; Point2DPT p2 = data[i];
			pen.setWidthF(width * p1.pressure()); painter->setPen(pen);
			painter->drawLine(p1,p2);
		}
    }

	void GraphicsPathItem::push_back(QPointF point, float pressure, int time)
    {
		Point2D pt = parentStroke->inverseTransform().map(point);
		parentStroke->push_back(Point2DPT(pt.x(),pt.y(), pressure, time));
    }

    void GraphicsPathItem::push_back(Point2DPT point)
    {
		Point2D pt = parentStroke->inverseTransform().map(point);
		point.setX(pt.x()); point.setY(pt.y());
		parentStroke->push_back(point);
    }

    void GraphicsPathItem::ApplySmoothing(int order)
    {
		parentStroke->applySmoothing(order);
    }

    bool GraphicsPathItem::Selected(QPointF point, float extraWidth)
    {
		Point2D p = Point2D(point.x(),point.y());
		bool val = parentStroke->containsPoint(p, SelectionType::OnItem, extraWidth);
        return val;
    }

    void GraphicsPathItem::OnStrokeUpdate(Stroke* stroke)
    {
		Q_UNUSED(stroke);/*
        painterPath = QPainterPath(QPointF(parentStroke->points[0]->x,parentStroke->points[0]->y));
        for (int i=1; i<parentStroke->points.size();i++)
			painterPath.lineTo(parentStroke->points[i]->x(),parentStroke->points[i]->y());
		setPath(painterPath);*/
        update();
    }
}
