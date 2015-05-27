#include "gesturepoint2d.h"

namespace CDI
{
    GesturePoint2D::GesturePoint2D(float X, float Y, int ID)
        : Point2D(X,Y), strokeID(ID) {}

    GesturePoint2D::GesturePoint2D(Point2D* point, int ID)
		:Point2D(point->x(),point->y()), strokeID(ID) {}
}
