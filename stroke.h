#pragma once
#include "cdicommon.h"
#include <vector>

using namespace std;

namespace CDI
{
    class Point2DPT;

    class Stroke : public Shape
    {
    protected:
        Type p_type;
    public:
        vector<Point2DPT*> points;

        bool closed;

        Stroke();

        Stroke(vector<Point2DPT*> &pointList);

        Stroke(int len, Point2DPT* pointList);

        ~Stroke();

        virtual Shape* Clone();

        void ApplySmoothing(int order);
    };

    // Point2DPV add extra information about pressure and time information
    // Can we use it to create an undo chain??
    class Point2DPT : public Point2D
    {
    public:
        int _pressure;
        long _time;

        Point2DPT();

        Point2DPT(float X, float Y, int pressure = 0, long time = 0);

        Point2DPT(Point2D p, int pressure = 0, long time = 0);

        //Point2DPT(Point2DPT &orig);
    };

}
