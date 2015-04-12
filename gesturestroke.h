#pragma once

#include "point2d.h"
#include<vector>

using namespace std;

namespace CDI
{
    class GestureStroke
    {
    public:
        vector<Point2D*> strokePoints;

        GestureStroke();

        ~GestureStroke();

    };
}
