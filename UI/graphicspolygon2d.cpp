#include "graphicspolygon2d.h"
#include <QPainter>
#include <QPainterPath>
#include <QVectorIterator>
#include <QDebug>
#include "graphicsitemgroup.h"

using namespace std;

namespace CDI
{
//	// TODO - Add stroke and thickness information to the stroke
//	GraphicsPolygon2D::GraphicsPolygon2D(QGraphicsItem *parent, Point2D startPoint, float pressure, int time)
//		: QGraphicsPathItem (parent)
//	{
//		parentPolygon = new Polygon2D();
//		parentPolygon->push_back(Point2DPT(startPoint.x(), startPoint.y(),pressure,time));
//	}

	GraphicsPolygon2D::GraphicsPolygon2D(GraphicsItemGroup *parent, Polygon2D *polygon)
		: QGraphicsPathItem (parent)
	{
		parentPolygon = polygon;
		update();
	}

	GraphicsPolygon2D::~GraphicsPolygon2D()
	{

	}

	QRectF GraphicsPolygon2D::boundingRect() const
	{
		if (parentPolygon == NULL) return QRectF();
		return parentPolygon->boundingRect();
	}

	void GraphicsPolygon2D::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
	{
		Q_UNUSED(widget)
		Q_UNUSED(option)
		if (parentPolygon == NULL) return;

		if (parentPolygon->size() < 2) return;

		QPen pen = this->pen();
		pen.setColor(parentPolygon->color());
		float width = parentPolygon->thickness();
		pen.setWidthF(width);
		painter->setPen(pen);

		Point2D* data = parentPolygon->data();		// gives out the data. Careful not to overwrite that information
		int num_points = parentPolygon->size();
		painter->setTransform(parentPolygon->transform());

		for (int i=1; i< num_points; i++)
		{
			Point2D p1 = data[i-1]; Point2D p2 = data[i];
			painter->drawLine(p1,p2);
		}
	}

	void GraphicsPolygon2D::push_back(QPointF point)
	{
		if (parentPolygon == NULL) return;
		Point2D pt = parentPolygon->inverseTransform().map(point);
		parentPolygon->push_back(pt);
	}

	void GraphicsPolygon2D::push_back(Point2DPT point)
	{
		if (parentPolygon == NULL) return;
		Point2D pt = parentPolygon->inverseTransform().map(point);
		parentPolygon->push_back(pt);
	}

	void GraphicsPolygon2D::ApplySmoothing(float margin)
	{
		if (parentPolygon == NULL) return;
		qDebug() << "Applying smoothing to polygon";
		parentPolygon->applyRDPSmoothing(margin);
	}

	bool GraphicsPolygon2D::containsPoint(const Point2D &pt, SelectionType rule, float margin)
	{
		if (parentPolygon == NULL) return false;
		return parentPolygon->containsPoint(pt, Qt::WindingFill);
	}

	bool GraphicsPolygon2D::Selected(QPointF point, float extraWidth)
	{
		if (parentPolygon == NULL) return false;
		Point2D p = Point2D(point.x(),point.y());
		bool val = parentPolygon->containsPoint(p, Qt::WindingFill);
		return val;
	}

	void GraphicsPolygon2D::updatePolygon()
	{
		if (parentPolygon!= NULL)
			setTransform(parentPolygon->transform());
	}
}
