#pragma once
#include "commonfunctions.h"
#include <QDataStream>
#include <QDebug>
#include <QTransform>

class QTransform;
namespace CDI
{
	class Point2DPT : public Point2D
	{
	protected:
		friend class QTransform;
		friend class QMatrix;

		float _pressure;
		int _time;
	public:

		inline Point2DPT();
		inline Point2DPT(const Point2DPT& point);
		inline Point2DPT(float x, float y, float pressure, int time = 0);

		inline float pressure() const;
		inline int time() const;
		inline void setPressure(float p);
		inline void setTime(int t);

		friend inline const Point2DPT operator*(float, const Point2DPT&);
		friend inline const Point2DPT operator*(const Point2DPT&, float);
		friend inline const Point2DPT operator+(const Point2DPT&, const Point2DPT&);
		friend inline const Point2DPT operator-(const Point2DPT&, const Point2DPT&);
		friend inline const Point2DPT operator*(const QTransform&, const Point2DPT&);

		friend QDataStream &operator <<(QDataStream &stream, const Point2DPT &point);
		friend QDataStream &operator >>(QDataStream &stream, Point2DPT &point);
		friend QDebug operator <<(QDebug d, const Point2DPT &p);
	};

	/**********************************************************
	* Point2DPT inline functions
	**********************************************************/
	inline Point2DPT::Point2DPT()
		: Point2D(),
		  _pressure(1.0), _time(0){}

	inline Point2DPT::Point2DPT(const Point2DPT& point)
		: Point2D(point.x(),point.y()),
		  _pressure(point.pressure()), _time(point.time()) {}

	inline Point2DPT::Point2DPT(float x, float y, float pressure, int time)
		: Point2D (x,y),
		  _pressure(pressure), _time(time) {}

	inline float Point2DPT::pressure() const
	{
		return _pressure;
	}

	inline int Point2DPT::time() const
	{
		return _time;
	}

	inline void Point2DPT::setPressure(float p)
	{
		_pressure = p;
	}

	inline void Point2DPT::setTime(int t)
	{
		_time = t;
	}

	/********************************************************
	* Point2DPT operator overload function
	*******************************************************/
	inline const Point2DPT operator* (float f, const Point2DPT& p)
	{
		return Point2DPT(p.x()*f,p.y()*f,p.pressure(),p.time());
	}

	inline const Point2DPT operator*(const Point2DPT& p, float f)
	{
		return Point2DPT(p.x()*f,p.y()*f,p.pressure(),p.time());
	}

	inline const Point2DPT operator+(const Point2DPT& p1, const Point2DPT& p2)
	{
		return Point2DPT(p1.x()+p2.x(), p1.y()+p2.y(),0.5f*(p1.pressure()+p2.pressure()), p1.time());
	}

	inline const Point2DPT operator-(const Point2DPT& p1, const Point2DPT& p2)
	{
		return Point2DPT(p1.x()-p2.x(), p1.y()-p2.y(),0.5f*(p1.pressure()+p2.pressure()), p1.time());
	}

	inline const Point2DPT operator*(const QTransform& t, const Point2DPT& p)
	{
		qreal *xt, *yt;
		t.map(p.x(),p.y(),xt,yt);
		return Point2DPT(*xt,*yt, p.pressure(),p.time());
	}

}
