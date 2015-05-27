#include "gesturestroke.h"

using namespace std;

namespace CDI {
    GestureStroke::GestureStroke()
    {
        strokePoints = vector<Point2D*>();
    }

    GestureStroke::~GestureStroke()
    {
        for (vector<Point2D*>::iterator it = strokePoints.begin();
             it < strokePoints.end();
             it++)
            delete *it;
        strokePoints.clear();
    }
}
