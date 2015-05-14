#include "cdigraphicspathitem.h"
#include <QPainter>
#include <QPainterPath>
#include <vector>
#include <QDebug>

using namespace std;

namespace CDI
{
    GraphicsPathItem::GraphicsPathItem(QGraphicsItem *parent, QPointF startPoint, float pressure, long time)
        : QGraphicsPathItem(parent)
    {
        painterPath = QPainterPath(startPoint);
        parentStroke = new Stroke(NULL);
        parentStroke->points.push_back(new Point2DPT(startPoint.x(), startPoint.y(),pressure,time));
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

    void GraphicsPathItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
    {
        QPen _pen = pen(); qreal width = _pen.widthF();
        if (parentStroke->points.size() < 3) return;
        vector<Point2DPT*>::iterator it = parentStroke->points.begin();
        Point2DPT* p1 = (*it); ++it; Point2DPT* p2 = (*it);
        for (; it != parentStroke->points.end(); ++it)
        {
            _pen.setWidthF(width * p1->pressure);
            painter->setPen(_pen);
            painter->drawLine(QPointF(p1->x,p1->y), QPointF(p2->x,p2->y));
            p1 = p2;
            p2 = (*it);
        }

        //QGraphicsPathItem::paint(painter, option, widget);
    }

    void GraphicsPathItem::push_back(QPointF point, float pressure, long time)
    {
        parentStroke->points.push_back(new Point2DPT(point.x(), point.y(), pressure,0));
        parentStroke->update();
    }

    void GraphicsPathItem::push_back(Point2DPT point)
    {
        parentStroke->points.push_back(new Point2DPT(point,point.pressure,point.time));
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
