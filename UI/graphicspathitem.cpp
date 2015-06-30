#include "graphicspathitem.h"
#include <QPainter>
#include <QPainterPath>
#include <QVectorIterator>
#include "QsLog.h"
#include <QDebug>
#include "graphicsitemgroup.h"

using namespace std;

namespace CDI
{

	// TODO - Add stroke and thickness information to the stroke
//	GraphicsPathItem::GraphicsPathItem(QGraphicsItem *parent, Point2D startPoint, float pressure, int time)
//		: QGraphicsPathItem (parent)
//	{
//		_parentStroke = new Stroke();
//		_parentStroke->setTransform
//				(_parentStroke->transform().translate(startPoint.x(), startPoint.y()));
//		setTransform(_parentStroke->transform());
//		push_back(startPoint,pressure,time);
//    }

	GraphicsPathItem::GraphicsPathItem(GraphicsItemGroup *parent, Stroke *stroke)
		: QGraphicsItem (parent)
    {
		_parentStroke = stroke;
		_pen = QPen();
		_pen.setColor(_parentStroke->color());
		_pen.setWidthF(_parentStroke->thickness());
		_brush = QBrush(Qt::NoBrush);

        _highlighted = false;

		update();
    }

    GraphicsPathItem::~GraphicsPathItem()
    {
		// Do not delete the __parentStroke object here. This is deleted when _parentStroke object is deleted
    }

	QBrush GraphicsPathItem::brush() const
	{
		return _brush;
	}

	QPen GraphicsPathItem::pen() const
	{
		return _pen;
	}

	Stroke* GraphicsPathItem::parentStroke() const
	{
		return _parentStroke;
	}

	void GraphicsPathItem::setBrush(const QBrush &brush)
	{
		_brush = QBrush(brush);
	}

	void GraphicsPathItem::setPen(const QPen &pen)
	{
		_pen = QPen(pen);
	}

	void GraphicsPathItem::setParentStroke(Stroke *stroke)
	{
		if (stroke!= NULL)
		{
			_parentStroke = stroke;
			setTransform(_parentStroke->transform());
		}
	}

    void GraphicsPathItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
		Q_UNUSED(widget)
		Q_UNUSED(option)
		// Hide when hidden
		if (_parentStroke->isVisible() == false) return;
		if (_parentStroke->size() < 3) return;

		painter->setBrush(_brush);
		QPen pen = QPen(_pen);

		float width = _parentStroke->thickness();

		Point2DPT* data = _parentStroke->data();		// gives out the data. Careful not to overwrite that information
		int num_points = _parentStroke->size();
        if (_highlighted)
        {
            QPen highlighter = QPen(_pen);
            highlighter.setWidthF(width * 1.5f);
            highlighter.setColor(Qt::red);
            painter->setPen(highlighter);
            for (int i=1; i< num_points; i++)
            {
                Point2DPT p1 = data[i-1]; Point2DPT p2 = data[i];
                painter->drawLine(p1, p2);
            }
        }
		for (int i=1; i< num_points; i++)
		{
			Point2DPT p1 = data[i-1]; Point2DPT p2 = data[i];
			pen.setWidthF(width * p1.pressure()); painter->setPen(pen);
			painter->drawLine(p1,p2);
		}
    }

	bool GraphicsPathItem::contains(QPointF point)
	{
		return Selected(point, 0.0f);
	}


	void GraphicsPathItem::push_back(QPointF point, float pressure, int time)
    {
		if (_parentStroke!= NULL)
		{
			Point2D pt = mapFromScene(point);
			_parentStroke->push_point(Point2DPT(pt.x(),pt.y(), pressure, time));
		}
    }

    void GraphicsPathItem::push_back(Point2DPT point)
    {
		if (_parentStroke!= NULL)
		{
			Point2D pt = mapFromScene(point);
			point.setX(pt.x()); point.setY(pt.y());
			_parentStroke->push_point(point);
		}
    }

    void GraphicsPathItem::ApplySmoothing(int order)
    {
		if (_parentStroke!= NULL) _parentStroke->applySmoothing(order);
    }

    bool GraphicsPathItem::Selected(QPointF point, float extraWidth)
    {
		if (_parentStroke== NULL) return false;
		Point2D p = Point2D(point.x(),point.y());
		bool val = _parentStroke->containsPoint(p, OnItem, extraWidth);
        return val;
    }

	QRectF GraphicsPathItem::boundingRect() const
	{
		if (_parentStroke== NULL) return QRectF();
		return _parentStroke->boundingRect();
	}

	void GraphicsPathItem::updateStroke()
    {
		if (_parentStroke!= NULL)
		{
			setTransform(_parentStroke->transform());
			_pen.setColor(_parentStroke->color());
			_pen.setWidthF(_parentStroke->thickness());
		}
    }

    bool GraphicsPathItem::isHighlighted() const
    {
        return _highlighted;
    }

    void GraphicsPathItem::highlight(bool value)
    {
        if (_highlighted == value) return;
        _highlighted = value;
        update(boundingRect());
    }
}
