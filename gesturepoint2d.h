#pragma once

#include "point2d.h"
namespace CDI
{
    class GesturePoint2D : public Point2D
    {
    public:
        int strokeID;

        GesturePoint2D(float X, float Y, int ID = 0);

        GesturePoint2D(Point2D* point, int ID);
    };
}