#pragma once

#include "point2d.h"
#include<vector>

using namespace std;

namespace CDI
{
    class GestureStroke
    {
    public:
        // NOTE - we do not need pressure and time information in this case
        vector<Point2D*> strokePoints;

        GestureStroke();

        ~GestureStroke();

    };
}
