#include "polygon2d.h"
#include "commonfunctions.h"
#include "ramerdouglaspeucker.h"
#include "vector"

namespace CDI
{
	void Polygon2D::setAsConvex(bool enableInternalCheck)
	{
		if (enableInternalCheck)
			_isConvex = checkConvexity();
		else
			_isConvex = true;
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

	bool Polygon2D::containsPoint(const Point2D &pt, SelectionType rule, float margin)
	{
		Q_UNUSED(rule)
		Q_UNUSED(pt)
		Q_UNUSED(margin)

//		Point2D relPos = inverseTransform().map(pt);

//		qDebug() << "Before" << pt << "After" << relPos;
//		float width = _thickness + margin;
//		float sqrWidth = width*width;
//		int num_points = size() - 1;
//		Point2DPT* points = data();
//		for (int i=0; i< num_points; i++)
//		{
//			Point2DPT p1 = points[i];
//			Point2DPT p2 = points[i+1];

//			Point2D v1 = Point2D(relPos-p1);
//			Point2D v2 = Point2D(p2-p1);
//			float v2SqrMag = sqrMagnitude(&v2);

//			float t = dotProduct(&v1, &v2) / v2SqrMag;
//			if (t< 0.0f || t > 1.0f) continue;
//			v2 = v2*t;
//			float dist = sqrEuclideanDistance(&v1, &v2);
//			if (dist < sqrWidth) return true;
//		}
		return false;
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

		qDebug() << "Initial size" << ptvec.size();
		ptvec = rdp.simplifyWithRDP(ptvec, margin);
		qDebug() << "Final size" << ptvec.size();

		clear();
		for (int i=0; i<ptvec.size(); i++)
		{
			Point2D p = ptvec[i];
			push_back(p);
		}
	}

	ItemType Polygon2D::type() const
	{
		return ItemType::POLYGON2D;
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
