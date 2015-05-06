#include "stroke.h"

namespace CDI
{
    // Our assumption is that there is no transformation during the creation process?
    Stroke::Stroke(QObject *parent) : Shape(parent)
    {
		thickness = 3;
        p_parent = parent;
        p_type = STROKE;
        closed = false;
        points = vector<Point2DPT*>();
    }

    Stroke::Stroke(QObject *parent, vector<Point2DPT *> &pointList) : Shape(parent)
    {
		thickness = 3;
        transform = QTransform();
        p_parent = parent;
        p_type = STROKE;
        closed = false;
        points = vector<Point2DPT*>();
        for (int i=0; i< pointList.size(); i++)
        {
            points.push_back(
                        new Point2DPT(pointList[i]->x, pointList[i]->y,
                                      pointList[i]->_pressure, pointList[i]->_time));
        }
    }

    Stroke::Stroke(QObject *parent, int len, Point2DPT *pointList) : Shape(parent)
    {
		thickness = 3;
        p_parent = parent;
        p_type = STROKE;
        closed = false;
        points = vector<Point2DPT*>();
        for (int i=0; i< len; i++)
        {
            points.push_back(new Point2DPT(
                                 pointList[i].x, pointList[i].y,
                                 pointList[i]._pressure, pointList[i]._time));
        }
    }

    Stroke::~Stroke()
    {
        // Delete the points stored in the stroke.
        for(vector<Point2DPT*>::iterator it = points.begin();
            it < points.end();
            it++)
            delete *it;
        points.clear();
    }

    Shape* Stroke::Clone()
    {
        Stroke* newStroke = new Stroke(p_parent, points);
        return newStroke;
    }

    void Stroke::ApplySmoothing(int order)
    {
        int n_points = points.size() -1;
        for (int i =0;i<order;i++)
        {
            for (int index=1; index < n_points; index++)
            {
                points[index]->x = 0.5* (points[index-1]->x +points[index+1]->x);
                points[index]->y = 0.5* (points[index-1]->y +points[index+1]->y);
            }
        }
        update();
    }

    void Stroke::update()
    {
        emit ItemChanged(this);
    }

	bool Stroke::Selected(Point2D* p, float extraWidth)
	{
		float width = thickness + extraWidth;
		float sqrWidth = width*width;
		int n_points = points.size()-1;
		// TODO - We can check for this as well
		if (n_points < 1) return false;

		bool result = false;
		for (int i=0; i< n_points; i++)
		{
			Point2D* p1 = points[i];
			Point2D* p2 = points[i+1];

			Point2D V1 = Point2D(p->x-p1->x, p->y-p1->y);
			Point2D V2 = Point2D(p2->x-p1->x,p2->y-p1->y);
			float sqrMag = (V2.x*V2.x + V2.y*V2.y);

			// dot product
			float t = (V1.x*V2.x + V1.y*V2.y) / sqrMag;
			if (t < 0.0 || t > 1.0) continue;

			// check normal distance
			V2.x = V2.x*t; V2.y = V2.y*t;	// set to coord of projection point
			float dist = (V1.x-V2.x)*(V1.x-V2.x) + (V1.y-V2.y)*(V1.y-V2.y);
			if (dist < sqrWidth) return true;
		}
		return false;
	}

    /* ---------------------------------------------------------------------------------
     * Pont2DPT class contains extra informaion which might be utilized by some gesture
     * recognizers as well as can be used as rendering aid.
     * ------------------------------------------------------------------------------ */
    Point2DPT::Point2DPT() :
        Point2D(0,0), _pressure(0), _time(0)
    {

    }

    Point2DPT::Point2DPT(float X, float Y, int pressure, long time) :
        Point2D(X,Y), _pressure(pressure), _time(time)
    {

    }


    Point2DPT::Point2DPT(Point2D p, int pressure, long time) :
        Point2D(p.x,p.y), _pressure(pressure), _time(time)
    {

    }

}
