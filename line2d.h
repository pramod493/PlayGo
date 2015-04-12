#pragma once

#include "point2d.h"
#include "cdicommon.h"

namespace CDI
{

    class Line2D : public Shape
    {
    protected:
        Type p_type;
    public:
        Point2D start, end;

        Line2D(Point2D, Point2D);

        Line2D();

        virtual Shape* Clone();
    };
}
