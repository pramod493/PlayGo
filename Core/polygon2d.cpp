#include "polygon2d.h"
#include "component.h"
#include "commonfunctions.h"
#include "ramerdouglaspeucker.h"
#include "vector"
#include "QsLog.h"

namespace CDI
{
	Polygon2D::Polygon2D(Component *component)
		: _color(Qt::black), _thickness(3.0f), _isConvex(false)
	{
		setParentItem(component);
	}

	Polygon2D::Polygon2D(Component* component, QColor color, float thickness)
		: _color(color), _thickness(thickness), _isConvex(false)
	{
		setParentItem(component);
	}

	Polygon2D::Polygon2D(const Polygon2D &s)
		: QPolygonF(s) , _color(s.color()), _thickness(s.thickness()),
		  _isConvex(false)
	{
		setParentItem(s.parentItem());
	}

	Polygon2D::Polygon2D(Component* component, const QVector<Point2D>& points, QColor color, float thickness)
		: QPolygonF (points), _color(color), _thickness(thickness), _isConvex(false)
	{
		setParentItem(component);
	}

	Polygon2D::~Polygon2D()
	{
		if (parentItem() != NULL)
			parentItem()->removeItem(id());
	}

	QColor Polygon2D::color() const
	{
		return _color;
	}

	float Polygon2D::thickness() const
	{
		return _thickness;
	}

	void Polygon2D::setColor(QColor color)
	{
		_color = color;
	}

	void Polygon2D::setThickness(float thickness)
	{
		mask |= isModified;
		_thickness = thickness;
	}

	QRectF Polygon2D::boundingRect() const
	{
		return Polygon2D::boundingRect();
	}

	bool Polygon2D::convex()
	{
		return _isConvex;
	}

	bool Polygon2D::containsPoint(const Point2D &pt, Qt::FillRule rule)
	{
		return QPolygonF::containsPoint(pt, rule);
	}

	void Polygon2D::setAsConvex(bool enableInternalCheck)
	{
		if (enableInternalCheck)
			_isConvex = checkConvexity();
		else
			_isConvex = true;
	}



	ItemType Polygon2D::type() const
	{
		return POLYGON2D;
	}

	QTransform Polygon2D::transform() const
	{
		return _transform;
	}

	void Polygon2D::setTransform(QTransform transform)
	{
		mask |= isTransformed;
		_transform = transform;
		_inverseTransform = transform.inverted();
	}

	QTransform Polygon2D::inverseTransform() const
	{
		return _inverseTransform;
	}

	QDataStream& Polygon2D::serialize(QDataStream &stream) const
	{
		int len = this->size();
		int i;
		stream << color();
		stream << float(thickness());
		stream << transform();
		stream << _isConvex;

		stream << qint32(len);
		for (i=0; i< len; i++)
			stream << this->at(i);
		return stream;
	}

	QDataStream& Polygon2D::deserialize(QDataStream &stream)
	{
		qint32 len =0;
		QTransform t;

		stream >> _color;
		stream >> _thickness;
		stream >> t;
		stream >> _isConvex;

		stream >> len;
		Point2DPT p;
		for (int i=0; i< len; i++)
		{
			stream >> p;
			push_back(p);
		}
		setTransform(t);

		mask |= isModified;

		return stream;
	}

	void Polygon2D::translate(float x, float y)
	{
		translate(QPointF(x,y));
	}


	void Polygon2D::translate(const Point2D &offset)
	{
		if (offset.isNull())
			return;

		mask |= isTransformed;

		Point2D *p = data();
		int i = size();
		while (i--) {
			*p += offset;
			++p;
		}
	}

	void Polygon2D::applyRDPSmoothing(float margin)
	{
		std::vector<Point2D> ptvec;
		Point2D* points = data();
		int num_points = size();
		for (int i=0; i<num_points; i++)
		{
			ptvec.push_back(points[i]);

		}
		RamerDouglas rdp;

		QLOG_INFO() << "Initial size" << ptvec.size();
		ptvec = rdp.simplifyWithRDP(ptvec, margin);
		QLOG_INFO() << "Final size" << ptvec.size();

		clear();
		for (int i=0; i<ptvec.size(); i++)
		{
			Point2D p = ptvec[i];
			push_back(p);
		}
	}

	bool Polygon2D::checkConvexity() const
	{
		return false;
//		Point2D* points = data();
//		int numPoints = size();
//		return isConvexPolygon(data(), numPoints);
	}

	QDebug operator <<(QDebug d, const Polygon2D &Polygon2D)
	{
		d.nospace() << "Polygon2D id: " << Polygon2D.id().toString();
		d.nospace() << "\n";
		d.nospace() << "[Color:"<<Polygon2D.color()<<"Thickness:"
					<< Polygon2D.thickness() << "Size:"<<Polygon2D.size() << "]";
		d.nospace() << "\n";
		d.nospace() << Polygon2D.transform();
		d << "\n";
		QVector<Point2D> v = Polygon2D;
		d.nospace() << v;
		return d.space();
	}
}
