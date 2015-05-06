#pragma once
#include "cdicommon.h"
#include <vector>
#include <QObject>
#include <QTransform>

using namespace std;

namespace CDI
{
    class Point2DPT;

    class Stroke : public Shape
    {
        Q_OBJECT
    protected:
        Type p_type;

        QObject* p_parent;
    public:
		int thickness;

        QTransform transform;

        vector<Point2DPT*> points;

        bool closed;

        Stroke(QObject* parent);

        Stroke(QObject* parent, vector<Point2DPT*> &pointList);

        Stroke(QObject* parent, int len, Point2DPT* pointList);

        ~Stroke();

        virtual Type GetType() { return p_type;}

        virtual Shape* Clone();

        void ApplySmoothing(int order);

        void update();

		bool Selected(Point2D* p, float extraWidth);

    signals:
        void ItemChanged(Stroke*);
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
