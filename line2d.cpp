#include "line2d.h"

namespace CDI
{
    Line2D::Line2D(Point2D a, Point2D b)
    {
        p_type = LINE;
        start = Point2D(a.x,a.y);
        end = Point2D(b.x,b.y);
    }

    Line2D::Line2D()
    {
        p_type = LINE;
        start = Point2D(0,0);
        end = Point2D(0,0);
    }

    Shape* Line2D::Clone()
    {
        Line2D* copy = new Line2D(start,end);
        return copy;
    }
}
