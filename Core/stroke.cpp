#include "stroke.h"

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
		Q_UNUSED(margin); Q_UNUSED(rule);
		qDebug() << "Feature not implemented" << pt;
		return false;
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
