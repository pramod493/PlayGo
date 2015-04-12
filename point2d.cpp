#include "point2d.h"

namespace CDI
{
    Point2D::Point2D()
        :x(0), y(0)
    {

    }

    Point2D::Point2D(float X, float Y)
        :x(X), y(Y)
    {

    }

    Point2D::Point2D(Point2D& orig)
        : x(orig.x), y(orig.y)
    {

    }
}
