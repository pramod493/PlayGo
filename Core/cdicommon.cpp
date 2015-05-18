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

	float DistanceFromLine(Point2D* p, Point2D* lineStart, Point2D* lineEnd)
	{
		Point2D V1 = Point2D(p->x-lineStart->x, p->y-lineStart->y);
		Point2D V2 = Point2D(lineEnd->x-lineStart->x,lineEnd->y-lineStart->y);
		float sqrMag = (V2.x*V2.x + V2.y*V2.y);

		// dot product
		float t = (V1.x*V2.x + V1.y*V2.y) / sqrMag;
//		if (t < 0.0 || t > 1.0) continue;

		// check normal distance
		V2.x = V2.x*t; V2.y = V2.y*t;	// set to coord of projection point
		float dist = (V1.x-V2.x)*(V1.x-V2.x) + (V1.y-V2.y)*(V1.y-V2.y);
		return dist;
	}

	bool DistanceFromLineSegment(Point2D* p, Point2D* lineStart, Point2D* lineEnd, float* distance)
	{
		bool OnSegment = true;
		Point2D V1 = Point2D(p->x-lineStart->x, p->y-lineStart->y);
		Point2D V2 = Point2D(lineEnd->x-lineStart->x,lineEnd->y-lineStart->y);
		float sqrMag = (V2.x*V2.x + V2.y*V2.y);

		// dot product
		float t = (V1.x*V2.x + V1.y*V2.y) / sqrMag;
		if (t < 0.0 || t > 1.0) OnSegment = false;


		if (t < 0.0)
		{
			*distance = EuclideanDistance(p, lineStart);
		} else if (t > 1.0)
		{
			*distance = EuclideanDistance(p, lineEnd);
		} else {
			// check normal distance
			V2.x = V2.x*t; V2.y = V2.y*t;	// set to coord of projection point
			*distance = sqrt ((V1.x-V2.x)*(V1.x-V2.x) + (V1.y-V2.y)*(V1.y-V2.y));
		}
		return OnSegment;
	}

    bool ColorCompare(QColor c1, QColor c2)
    {
        return (c1.red()== c2.red()) && (c1.green() == c2.green()) && (c1.blue() == c2.blue());
    }
}
