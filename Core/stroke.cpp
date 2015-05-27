#include "stroke.h"
#include "commonfunctions.h"
#include "ramerdouglaspeucker.h"
#include "vector"

namespace CDI
{
	void Stroke::translate(const Point2D &offset)
	{
		if (offset.isNull())
			return;

		mask |= isTransformed;

		Point2DPT *p = data();
		int i = size();
		while (i--) {
			*p += offset;
			++p;
		}
	}

	bool Stroke::containsPoint(const Point2D &pt, SelectionType rule, float margin)
	{
		Q_UNUSED(rule)

		Point2D relPos = inverseTransform().map(pt);

		qDebug() << "Before" << pt << "After" << relPos;
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

	void Stroke::applySmoothing(int order)
	{
		if (0) {
			std::vector<Point2D> ptvec;
			Point2DPT* points = data();
			int num_points = size();
			for (int i=0; i<num_points; i++)
			{
				ptvec.push_back(Point2D(points[i].x(),points[i].y()));

			}
			RamerDouglas rdp;

			qDebug() << "Initial size" << ptvec.size();
			ptvec = rdp.simplifyWithRDP(ptvec, 3);
			qDebug() << "Final size" << ptvec.size();

			clear();
			for (int i=0; i<ptvec.size(); i++)
			{
				Point2D p = ptvec[i];
				push_back(Point2DPT(p.x(),p.y(),1.0f,0));
			}

		}

		if (1) {
			int num_points = size()-1;
			Point2DPT* points = data();
			for (int j =0; j< order; j++)
				for (int index = 1;index < num_points; index++)
				{
					points[index] = 0.5f * (points[index-1] + points[index+1]);
				}
		}
	}

	ItemType Stroke::type() const
	{
		return ItemType::STROKE;
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

	QDebug operator <<(QDebug d, const Stroke &stroke)
	{
		d.nospace() << "Stroke id: " << stroke.id().toString();
		d.nospace() << "\n";
		d.nospace() << "[Color:"<<stroke.color()<<"Thickness:"
					<< stroke.thickness() << "Size:"<<stroke.size() << "]";
		d.nospace() << "\n";
		d.nospace() << stroke.transform();
		d << "\n";
		QVector<Point2DPT> v = stroke;
		d.nospace() << v;
		return d.space();
	}
}
