#include "stroke.h"

namespace CDI
{
    Stroke::Stroke()
    {
        p_type = STROKE;
        closed = false;
        points = vector<Point2DPT*>();
    }

    Stroke::Stroke(vector<Point2DPT *> &pointList)
    {
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

    Stroke::Stroke(int len, Point2DPT *pointList)
    {
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
        Stroke* newStroke = new Stroke(points);
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
