#include "polygon2d.h"
#include <QPainter>
#include <QPainterPath>
#include <QVectorIterator>
#include "QsLog.h"

using namespace std;

namespace CDI
{
	Polygon2D::Polygon2D(QGraphicsItem* parent)
		: QGraphicsPolygonItem(parent)
	{
		_highlighted = false;
		_id = uniqueHash();
	}

	Polygon2D::Polygon2D(const QPolygonF& polygon, QGraphicsItem* parent)
		:QGraphicsPolygonItem(polygon, parent)
	{
		_highlighted = false;
		_id = uniqueHash();
	}

	Polygon2D::~Polygon2D()
	{

	}

	void Polygon2D::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
	{
		QGraphicsPolygonItem::paint(painter, option, widget);
		/*Q_UNUSED(widget)
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
		}*/
	}

	void Polygon2D::applySmoothing(float margin)
	{
		std::vector<Point2D> ptvec;
		QPolygonF poly = polygon();
		Point2D* points = poly.data();
		int num_points = poly.size();

		for (int i=0; i<num_points; i++)
			ptvec.push_back(points[i]);

		RamerDouglas rdp;
		QLOG_INFO() << "Initialize RDP smoothing. Initial Dim = " << ptvec.size();
		ptvec = rdp.simplifyWithRDP(ptvec, margin);
		QLOG_INFO() << "RDP complete. Dim = " << ptvec.size();

		poly.clear();
		for (int i=0; i<ptvec.size(); i++)
		{
			Point2D p = ptvec[i];
			poly.push_back(p);
		}
		setPolygon(poly);
	}

	bool Polygon2D::containsPoint(const Point2D &pt, SelectionType rule, float margin)
	{
		return polygon().containsPoint(pt, Qt::WindingFill);
	}

}
