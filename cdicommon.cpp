#include "cdicommon.h"

using namespace std;

namespace CDI
{
    bool FuzzyIsNull(float f)
    {
        return abs(f) <= 0.00001f;
    }

    float SqrEuclideanDistance(Point2D* a, Point2D* b)
    {
        return (a->x - b->x)*(a->x - b->x) +
                (a->y - b->y)*(a->y - b->y);
    }

    float EuclideanDistance(Point2D* a, Point2D* b)
    {
        return sqrt(SqrEuclideanDistance(a,b));
    }

    bool ColorCompare(QColor c1, QColor c2)
    {
        return (c1.red()== c2.red()) && (c1.green() == c2.green()) && (c1.blue() == c2.blue());
    }
}
