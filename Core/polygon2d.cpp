#include "polygon2d.h"
#include <QPainter>
#include <QPainterPath>
#include <QVectorIterator>
#include "QsLog.h"
#include "poly2tri.h"
#include "physicsshape.h"

using namespace std;

namespace CDI
{
	Polygon2D::Polygon2D(QGraphicsItem* parent)
		: QGraphicsPolygonItem(parent)
	{
		_highlighted = false;
		_id = uniqueHash();
		applyCustomRenderSettings = false;
		_internalPolygon = QPolygonF();
		_physicsShape = NULL;

		setZValue(Z_POLYGONVIEW);

#ifdef CDI_DEBUG_DRAW_SHAPE
		QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem(this);
		ellipse->setRect(QRectF(-10,-10,20,20));
		ellipse->setTransform(QTransform());
		ellipse->setBrush(QBrush(Qt::green));
#endif //CDI_DEBUG_DRAW_SHAPE
	}

	Polygon2D::Polygon2D(const QPolygonF& polygon, QGraphicsItem* parent)
		:QGraphicsPolygonItem(polygon, parent)
	{
		_highlighted = false;
		_id = uniqueHash();
		applyCustomRenderSettings = false;
		_internalPolygon = QPolygonF(polygon);
		_physicsShape = NULL;

		setZValue(Z_POLYGONVIEW);

#ifdef CDI_DEBUG_DRAW_SHAPE
		QGraphicsEllipseItem *ellipse = new QGraphicsEllipseItem(this);
		ellipse->setRect(QRectF(-10,-10,20,20));
		ellipse->setTransform(QTransform());

		ellipse->setBrush(QBrush(Qt::green));
#endif //CDI_DEBUG_DRAW_SHAPE
	}

	Polygon2D::Polygon2D(const Polygon2D& copy)
		: QGraphicsPolygonItem(copy.polygon(), copy.parentItem())
	{
		_id = uniqueHash();
		applyCustomRenderSettings = false;
		_internalPolygon = QPolygonF(copy._internalPolygon);
		_physicsShape = new PhysicsShape(*copy._physicsShape);

		setZValue(copy.zValue());

		setPen(copy.pen());
		setBrush(copy.brush());
	}

	Polygon2D::~Polygon2D()
	{
		_internalPolygon.clear();
	}

	void Polygon2D::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
	{
		QGraphicsPolygonItem::paint(painter, option, widget);
		if (_highlighted)
		{
			painter->setPen(QPen(Qt::DashDotDotLine));
			painter->setBrush(QBrush(Qt::NoBrush));
			painter->drawPolygon(_internalPolygon);
		}

#ifdef CDI_DEBUG_DRAW_SHAPE
		painter->setPen(QPen(Qt::black));
		painter->setBrush(QBrush(Qt::NoBrush));
		painter->drawRect(boundingRect());
#endif //CDI_DEBUG_DRAW_SHAPE
	}

	void Polygon2D::push_point(QPointF pos)
	{
		QPointF mappedPos = mapFromScene(pos);
		_internalPolygon.push_back(mappedPos);
		setPolygon(applyLaplacianSmoothing(_internalPolygon, 1));
	}

	PhysicsShape* Polygon2D::physicsShape() const
	{
		return _physicsShape;
	}

	QPolygonF Polygon2D::applyRDPSmoothing(QPolygonF poly, float margin)
	{
		std::vector<Point2D> ptvec;
		Point2D* points = poly.data();
		int num_points = poly.size();

		for (int i=0; i<num_points; i++)
			ptvec.push_back(points[i]);

		RamerDouglas rdp;
		ptvec = rdp.simplifyWithRDP(ptvec, margin);

		QPolygonF newPoly;
		for (int i=0; i<ptvec.size(); i++)
		{
			Point2D p = ptvec[i];
			newPoly.push_back(p);
		}
		return newPoly;
	}


	QPolygonF Polygon2D::applyLaplacianSmoothing(QPolygonF poly, int order)
	{
		QPolygonF _points = QPolygonF(poly);
		int num_points = _points.size()-1;
		QPointF* points = _points.data();
		for (int j =0; j< order; j++)
			for (int index = 1;index < num_points; index++)
			{
				points[index] = (points[index-1] + points[index+1]) * 0.5f;
			}
		return _points;
	}

	bool Polygon2D::containsPoint(const Point2D &pt, SelectionType rule, float margin)
	{
		Q_UNUSED(margin) Q_UNUSED(rule)
		return polygon().containsPoint(pt, Qt::WindingFill);
	}

	bool Polygon2D::isHighlighted() const
	{
		return _highlighted;
	}

	void Polygon2D::highlight(bool value)
	{
		if (_highlighted != value)
		{
			_highlighted = value;
		}
	}

	void Polygon2D::initializePhysicsShape()
	{
		_simplifiedPolygon = applyRDPSmoothing(polygon(), 6.0f);
		if (_physicsShape) delete _physicsShape;

		vector<p2t::Triangle*> p2tTrias = triangularizePolygon(_simplifiedPolygon);
		_physicsShape = new PhysicsShape(p2tTrias);
		for (int i=0; i < p2tTrias.size(); i++)
		{
			p2t::Triangle* tria = p2tTrias[i];
			if (tria) delete tria;
			p2tTrias[i] = 0;
		}
	}

	QDataStream& Polygon2D::serialize(QDataStream &stream) const
	{
		return stream;
	}

	QDataStream& Polygon2D::deserialize(QDataStream &stream)
	{
		return stream;
	}



}
