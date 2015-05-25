#include "point2dpt.h"

namespace CDI
{
    QDataStream & operator<<(QDataStream & stream, const Point2DPT & point)
    {
//		qDebug() << "Serialize " << point;
		stream << float(point.x()) << float(point.y()) << float(point.pressure()) <<
				  qint32(point.time());
        return stream;
    }

	QDataStream & operator >>(QDataStream & stream, Point2DPT & point)
	{
//		qDebug() << "Deserialize " << point;
		float x, y, pressure;
		qint32 time;
		stream >> x;
		stream >> y;
		stream >> pressure;
		stream >> time;

		point.setX(x);
		point.setY(y);
		point.setPressure(pressure);
		point.setTime(time);
		return stream;
	}

    QDebug operator<<(QDebug d, const Point2DPT &p)
    {
        d.nospace() << "CDI::Point2DPT(" << p.x() << ", " << p.y() <<
					   ", " << p.pressure() << ", " << p.time() <<')';
		return d;
	}

}
