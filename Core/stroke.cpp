#include "stroke.h"
#include "commonfunctions.h"
#include "ramerdouglaspeucker.h"
#include <QVectorIterator>

namespace CDI
{
	Stroke::Stroke(Component *component)
		: _color(Qt::black), _thickness(3.0f)
	{
		setParentItem(component);
		recalculateAABB();
	}

	Stroke::Stroke(Component* component, QColor color, float thickness)
		: _color(color), _thickness(thickness)
	{
		setParentItem(component);
		recalculateAABB();
	}

	Stroke::Stroke(const Stroke &s)
		: QVector<Point2DPT>(s) , _color(s.color()),
		  _thickness(s.thickness()), _transform(s.transform())
	{
		setParentItem(s.parentItem());
		recalculateAABB();
	}

	Stroke::Stroke(Component* component, const QVector<Point2DPT>& points, QColor color, float thickness)
		: QVector<Point2DPT> (points), _color(color),
		  _thickness(thickness)
	{
		setParentItem(component);
		recalculateAABB();
	}

	Stroke::~Stroke()
	{
		// Do something here
	}

	void Stroke::push_point(Point2DPT &pt)
	{
		push_back(pt);
		float px = pt.x();
		float py = pt.y();
		// if either if _x_min or _y_min is true, we do have to check the other one
		// A faster method can be thought of
		_x_min = (_x_min < px ? _x_min : px);
		_x_max = (_x_max > px ? _x_max : px);
		_y_min = (_y_min < py ? _y_min : py);
		_y_max = (_y_max > py ? _y_max : py);

		aabb = QRectF(_x_min, _y_min, _x_max - _x_min, _y_max - _y_min);
		aabb.setWidth(aabb.width()+2*_thickness);
		aabb.setHeight(aabb.height()+2*_thickness);
	}


	bool Stroke::containsPoint(const Point2D &pt, SelectionType rule, float margin)
	{
		Q_UNUSED(rule)

		Point2D relPos = inverseTransform().map(pt);
		float width = _thickness + margin;
		float sqrWidth = width*width;
		int num_points = size() - 1;
		Point2DPT* points = data();
		for (int i=0; i< num_points; i++)
		{
			Point2DPT p1 = points[i];
			Point2DPT p2 = points[i+1];

			Point2D v1 = Point2D(relPos-p1);
			Point2D v2 = Point2D(p2-p1);
			float v2SqrMag = sqrMagnitude(&v2);

			float t = dotProduct(&v1, &v2) / v2SqrMag;
			if (t< 0.0f || t > 1.0f) continue;
			v2 = v2*t;
			float dist = sqrEuclideanDistance(&v1, &v2);
			if (dist < sqrWidth) return true;
		}
		return false;
	}

	bool Stroke::isContainedWithin(QPolygonF *polygon, float percentmatch)
	{
		// NOTE This returns when all the points are inside the polygon
		// Make sure the polygon is mapped to the local coordinate system
		int num_points = size();
		Point2DPT* points = data();

		for (int i=0; i< num_points; i++)
		{
			if (polygon->contains(points[i]) == false) return false;
		}
		return true;
	}

	QRectF Stroke::boundingRect()
	{
		if (size() == 0) return QRectF();
		if (mask & isModified) updateWhenModified();
		return aabb;
		// Do not return the transformed AABB rectangle
	}

	ItemType Stroke::type() const
	{
		return ItemType::STROKE;
	}

	QTransform Stroke::transform() const
	{
		return _transform;
	}

	void Stroke::setTransform(QTransform t)
	{
		mask |= isTransformed;
		_transform = t;
		_inverseTransform = t.inverted();
	}

	QTransform Stroke::inverseTransform() const
	{
		return _inverseTransform;
	}

	QDataStream& Stroke::serialize(QDataStream &stream) const
	{
		int len = this->size();
		int i;
		stream << color();
		stream << float(thickness());
		stream << transform();
		stream << qint32(len);
		for (i=0; i< len; i++)
			stream << this->at(i);
		return stream;
	}

	QDataStream& Stroke::deserialize(QDataStream &stream)
	{
		qint32 len =0;
		QTransform t;

		stream >> _color;
		stream >> _thickness;
		stream >> t;
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

	void Stroke::translate(const Point2D &offset)
	{
		if (offset.isNull()) return;

		mask |= isModified;

		Point2DPT *p = data();
		int i = size();
		while (i--) {
			*p += offset;
			++p;
		}
	}

	void Stroke::applySmoothing(int order)
	{
		int num_points = size()-1;
		Point2DPT* points = data();
		for (int j =0; j< order; j++)
			for (int index = 1;index < num_points; index++)
			{
				points[index] = 0.5f * (points[index-1] + points[index+1]);
			}
	}

	bool Stroke::mergeWith(Stroke *stroke, bool joinAtEnd)
	{
		// Check thickness match
		if (!qFuzzyCompare(stroke->thickness(), _thickness))
			return false;
		if (!colorCompare(stroke->color(), _color))
			return false;
		if (!_transform.isIdentity() || !(stroke->transform().isIdentity()))
		{
			qDebug() << "Add transformation support later";
			return false;
		}

		if (joinAtEnd)
		{

		} else
		{

		}
		return false;
	}

	void Stroke::updateWhenModified()
	{
		recalculateAABB();
		mask &= ~isModified;
	}

	void Stroke::recalculateAABB()
	{
		if (size() == 0)
		{
			_x_max = _y_max = _x_min = _y_min = 0;
			aabb = QRectF();
			return;
		}
		int num_points = size();
		Point2DPT* points = data();

		_x_min=points[0].x(); _y_min=points[0].y();
		_x_max=points[0].x(); _y_max=points[0].y();

		for (int i=0; i< num_points; i++)
		{
			float px = points[i].x();
			float py = points[i].y();
			// if either if _x_min or _y_min is true, we do have to check the other one
			// A faster method can be thought of
			_x_min = (_x_min < px ? _x_min : px);
			_x_max = (_x_max > px ? _x_max : px);
			_y_min = (_y_min < py ? _y_min : py);
			_y_max = (_y_max > py ? _y_max : py);
		}
		aabb =  QRectF(_x_min, _y_min, _x_max - _x_min, _y_max - _y_min);
		aabb.setWidth(aabb.width()+2*_thickness);
		aabb.setHeight(aabb.height()+2*_thickness);
	}

	QDebug operator <<(QDebug d, const Stroke &stroke)
	{
		d.nospace() << "\nStroke id: " << stroke.id().toString();
		d.nospace() << "\n";
		d.nospace() << "[Color:"<<stroke.color()<<"Thickness:"
					<< stroke.thickness() << "Size:"<<stroke.size() << "]";
		d.nospace() << "\n\n";
		d.nospace() << stroke.transform();
		d << "\n";
		QVector<Point2DPT> v = stroke;
		d.nospace() << v;
		return d.space();
	}

	Stroke* merge(Stroke *s1, Stroke *s2, bool addS1Forward, bool addS2Forward)
	{
		Stroke* s12 = new Stroke(s1->parentItem());
		s12->setThickness(s1->thickness());
		s12->setColor(s1->color());
		{
			Point2DPT* data = s1->data();
			int num_points = s1->size();
			QTransform sTransform = s1->transform();

			for (int i=0; i< num_points; i++)
				s12->push_back(sTransform * data[i]);
		}

		{
			Point2DPT* data = s2->data();
			int num_points = s2->size();
			QTransform sTransform = s2->transform();

			for (int i=0; i< num_points; i++)
				s12->push_back(sTransform * data[i]);
		}
		return s12;
	}
}
