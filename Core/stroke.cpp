
#include <QVectorIterator>
#include "stroke.h"
#include "component.h"
#include "QsLog.h"
#include "commonfunctions.h"

using namespace std;

namespace CDI
{
	Stroke::Stroke(QGraphicsItem* parent)
		:QGraphicsPathItem(parent)
	{
		init();
		_points = QVector<Point2DPT*>();
		recalculateAABB();
		setZValue(Z_STROKEVIEW);
		_isStrokeFinalized = false;
	}

	Stroke::Stroke(QVector<Point2DPT*> points, QGraphicsItem* parent)
		:Stroke(parent)
	{
		_points.reserve(_points.size());
		for (auto pt : points)
		{
			Point2DPT* copy = new Point2DPT(*pt);
			_points.push_back(copy);
		}
		recalculateAABB();
	}

	Stroke::~Stroke()
	{
		if (auto graphicsitem = parentItem())
		{
			if (Component* component = qgraphicsitem_cast<Component*>(graphicsitem))
			{
				component->removeFromComponent(this);
			}
		}
		qDeleteAll(_points);
	}

	void Stroke::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
	{
		if (_isStrokeFinalized && !_highlighted)
		{
			QGraphicsPathItem::paint(painter, option, widget);
			return;
		}
		// NOTE - Seperate draw routine is invoked when drawn. Let it be so.

		if (!isVisible() || _points.size() < 3) return;

		painter->setBrush(brush());
		QPen _pen = QPen(pen());
		float width = _pen.widthF();
		// TODO - Get const_iterator
		Point2DPT** data = _points.data();		// gives out the data. Careful not to overwrite that information
		int num_points = _points.size();
		if (_highlighted)
		{
			QPen highlighter = QPen(_pen);
			highlighter.setWidthF(width * 1.5f);
			highlighter.setColor(Qt::red);
			painter->setPen(highlighter);
			for (int i=1; i< num_points; i++)
			{
				Point2DPT* p1 = data[i-1]; Point2DPT* p2 = data[i];
				painter->drawLine(*p1, *p2);
			}
		}
		for (int i=1; i< num_points; i++)
		{
			Point2DPT* p1 = data[i-1]; Point2DPT* p2 = data[i];
			_pen.setWidthF(width * p1->pressure()); painter->setPen(_pen);
			painter->drawLine(*p1, *p2);
		}
#ifdef CDI_DEBUG_DRAW_SHAPE
		painter->setPen((_pen.setWidth(0),_pen.setStyle(Qt::DashDotLine), _pen));
		painter->drawRect(boundingRect());
#endif
	}

	void Stroke::setStrokePath(QVector<Point2DPT *> points)
	{
		_points = points;
		recalculateAABB();
		finalize();
	}

	bool Stroke::contains(const QPointF &point) const
	{
		return contains(point, 0.0f);
	}

	bool Stroke::contains(const QPointF& point, float margin) const
	{
		Point2D pt = point;
		float width = pen().widthF() + margin;
		float sqrWidth = width*width;
		int num_points = _points.size() - 1;
		for (int i=0; i< num_points; i++)
		{
			Point2DPT* p1 = _points[i];
			Point2DPT* p2 = _points[i+1];

			Point2D v1 = Point2D(pt - (*p1));
			Point2D v2 = Point2D((*p2)-(*p1));
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
		QVector<Point2DPT*>::const_iterator iter;
		auto failedpoints = 0;
		for (iter = _points.constBegin();
			 iter != _points.constEnd();
			 ++iter)
		{
			Point2DPT* pt = (*iter);
			if (polygon->containsPoint(*pt, Qt::WindingFill) == false) failedpoints++;
		}
		QLOG_INFO() << "NUM POINTS" << _points.size() << "failed:" << failedpoints;
		// True as long as 90% is selected
		if (failedpoints < percentmatch * _points.size()) return true;
		return false;
	}

	QRectF Stroke::boundingRect() const
	{
		if (!_isStrokeFinalized)
		{
			if (_points.size() == 0) return QRectF();
			if (isVisible()== false) return QRectF();
			return aabb;
		}

		return QGraphicsPathItem::boundingRect();
	}

	void Stroke::push_point(QPointF point, float pressure, int time)
	{
		Point2D pt = mapFromScene(point);
		_points.push_back(new Point2DPT(pt.x(),pt.y(), pressure, time));
		updateAABB(pt.x(),pt.y());
	}

	void Stroke::push_point(Point2DPT point)
	{
		Point2D pt = mapFromScene(point);
		_points.push_back
				(new Point2DPT(pt.x(), pt.y(), point.pressure(), point.time()));
		updateAABB(pt.x(),pt.y());
	}

	void Stroke::applySmoothing(int order)
	{
		auto num_points = _points.size()-1;
		auto points = _points.data();
		for (int j =0; j< order; j++)
			for (int index = 1;index < num_points; index++)
			{
				points[index]->setX(0.5f *(points[index-1]->x() + points[index+1]->x()));
				points[index]->setY(0.5f *(points[index-1]->y() + points[index+1]->y()));
				points[index]->setPressure
						(0.5f *(points[index-1]->pressure() + points[index+1]->pressure()) );
			}

		finalize();
		recalculateAABB();

	}

	bool Stroke::isHighlighted() const
	{
		return _highlighted;
	}

	void Stroke::highlight(bool value)
	{
		if (_highlighted == value) return;
		_highlighted = value;
		update(boundingRect());
	}

	void Stroke::finalize()
	{
		auto data = _points.data();
		auto num_points = _points.size();
		if (num_points < 2) return;	// Need at least 2 points
		auto path = QPainterPath(*data[0]);
		for (auto i=1; i < num_points; i++)
		{
			path.lineTo(*data[i]);
		}
		setPath(path);
		_isStrokeFinalized = true;
	}

	QDataStream& Stroke::serialize(QDataStream &stream) const
	{
		stream << _id;
		stream << static_cast<int>(_points.size());
		for(QVector<Point2DPT*>::const_iterator iter = _points.constBegin();
			iter != _points.constEnd(); ++iter)
		{
			Point2DPT *point = (*iter);
			stream << *point;
		}
		stream << sceneTransform();
		stream << transform();
		return stream;
	}

	QDataStream& Stroke::deserialize(QDataStream &stream)
	{
		stream >> _id;
		return stream;
//		qint32 len =0;
//		QTransform t;
//		QPen _pen;
//
//		stream >> _pen;		setPen(_pen);
//		stream >> t;		setTransform(t);
//		stream >> len;

//		Point2DPT p;
//		for (int i=0; i< len; i++)
//		{
//			stream >> p;
//			_points.push_back(new Point2DPT(p));
//		}
//		recalculateAABB();
//		return stream;
	}

	void Stroke::init()
	{
		_id = uniqueHash();
		QPen _pen = QPen(Qt::black);
		_pen.setWidthF(3.0f);
		setPen(_pen);

		QBrush _brush = QBrush(Qt::NoBrush);
		setBrush(_brush);

		_highlighted = false;
	}

	void Stroke::updateAABB(float x, float y)
	{
		prepareGeometryChange();

		_x_min = (_x_min < x ? _x_min : x);
		_x_max = (_x_max > x ? _x_max : x);
		_y_min = (_y_min < y ? _y_min : y);
		_y_max = (_y_max > y ? _y_max : y);

		float margin = pen().widthF();
		aabb = QRectF(_x_min - margin, _y_min - margin,
					  _x_max - _x_min + 2.0f * margin, _y_max - _y_min + 2.0f * margin);
		update();
	}

	void Stroke::recalculateAABB()
	{
		if (_points.size() == 0)
		{
			_x_max = _y_max = _x_min = _y_min = 0;
			aabb = QRectF();
			return;
		}
		prepareGeometryChange();
		auto num_points = _points.size();
		Point2DPT** points = _points.data();

		_x_min=points[0]->x(); _y_min=points[0]->y();
		_x_max=points[0]->x(); _y_max=points[0]->y();

		for (int i=0; i< num_points; i++)
		{
			float px = points[i]->x();
			float py = points[i]->y();
			// if either if _x_min or _y_min is true, we do have to check the other one
			// A faster method can be thought of
			_x_min = (_x_min < px ? _x_min : px);
			_x_max = (_x_max > px ? _x_max : px);
			_y_min = (_y_min < py ? _y_min : py);
			_y_max = (_y_max > py ? _y_max : py);
		}
		float margin = pen().widthF();
		aabb = QRectF(_x_min - margin, _y_min - margin,
					  _x_max - _x_min + 2.0f * margin, _y_max - _y_min + 2.0f * margin);

	}
}
