#include "pdollargesture.h"
#include <algorithm>
#include <math.h>

using namespace std;

namespace CDI
{
    PDollarGesture::PDollarGesture(vector<GestureStroke *> strokes, string name)
    {
        gestureName = name;
        SAMPLING_RESOLUTION = 32;
        gesturePoints = vector<GesturePoint2D*>();
    }

    PDollarGesture::PDollarGesture(vector<GesturePoint2D*> points, string name)
    {
        gestureName = name;
        SAMPLING_RESOLUTION = 32;
        gesturePoints = vector<GesturePoint2D*>();
    }

    PDollarGesture::~PDollarGesture()
    {
        for(vector<GesturePoint2D*>::iterator it = gesturePoints.begin();
            it < gesturePoints.end();
            it++)
            delete *it;
        gesturePoints.clear();
    }

    void PDollarGesture::Scale()
    {
        float minx = CDI_MAX_FLOAT;
        float miny = CDI_MAX_FLOAT;
        float maxx = CDI_MIN_FLOAT;
        float maxy = CDI_MIN_FLOAT;
        size_t n = gesturePoints.size();

        for (int i=0; i < n; i++)
        {
            if (minx > gesturePoints[i]->x) minx = gesturePoints[i]->x;
            if (miny > gesturePoints[i]->y) miny = gesturePoints[i]->y;
            if (maxx < gesturePoints[i]->x) maxx = gesturePoints[i]->x;
            if (maxy < gesturePoints[i]->y) maxy = gesturePoints[i]->y;
        }

        float scale = max(maxx-minx, maxy-miny);
        for (int i=0; i < n; i++)
        {
            gesturePoints[i]->x = (gesturePoints[i]->x - minx)/scale;
            gesturePoints[i]->y = (gesturePoints[i]->y - miny)/scale;
        }
    }

    void PDollarGesture::TranslateTo(Point2D point)
    {
        size_t n = gesturePoints.size();
        for (int i=0; i < n; i++)
        {
            gesturePoints[i]->x -= point.x;
            gesturePoints[i]->y -= point.y;
        }
    }

    void PDollarGesture::Resample()
    {
        vector<GesturePoint2D*> resampledPoints = vector<GesturePoint2D*>();
        resampledPoints.push_back(new GesturePoint2D
                                  (gesturePoints[0]->x,gesturePoints[0]->y,
                                    gesturePoints[0]->strokeID ));
        int numPoints = 1;
        size_t n = gesturePoints.size();

        float I = PathLength()/SAMPLING_RESOLUTION;
        float D = 0;

        for (int i=1; i<n; i++)
        {
            if (gesturePoints[i]->strokeID == gesturePoints[i-1]->strokeID)
            {
                float d = EuclideanDistance(gesturePoints[i-1], gesturePoints[i]);
                if (D+d >= I)
                {
                    GesturePoint2D firstPoint = GesturePoint2D(gesturePoints[i-1]->x,
                            gesturePoints[i-1]->y,
                            gesturePoints[i-1]->strokeID);
                    while (D+d >= I)
                    {
                        float t = min(max((I-D)/d, 0.0f), 1.0f);
                        // TODO - not very sure on what to do in this case
                        if (abs(d) < 0.0001) t = 0.5f;
                        GesturePoint2D* newPoint =
                                new GesturePoint2D(
                                    (1.0f-t)*firstPoint.x + t*gesturePoints[i]->x,
                                    (1.0f-t)*firstPoint.y + t*gesturePoints[i]->y,
                                    firstPoint.strokeID);
                        resampledPoints.push_back(newPoint); numPoints++;

                        // update partial length
                        d = D+d-I;
                        D = 0;
                        firstPoint = GesturePoint2D(newPoint->x,newPoint->y,newPoint->strokeID);
                    }
                    D = d;
                }
                else D+=d;
            }
        }
        if (numPoints == SAMPLING_RESOLUTION-1)
            resampledPoints.push_back(
                        new GesturePoint2D(gesturePoints[n-1]->x, gesturePoints[n-1]->y,
                    gesturePoints[n-1]->strokeID));

        // Clear the current stroke and populate with the resampled value
        {
            for(vector<GesturePoint2D*>::iterator it = gesturePoints.begin();
                it < gesturePoints.end();
                it++)
                delete (*it);
            gesturePoints.clear();
            size_t n = resampledPoints.size();
            for (int i=0; i<n;i++) gesturePoints.push_back(resampledPoints[i]);
        }

    }

    Point2D PDollarGesture::Centroid()
    {
        float cx=0, cy=0;
        size_t n = gesturePoints.size();
        for (int i=0; i < n; i++)
        {
            cx+= gesturePoints[i]->x;
            cy += gesturePoints[i]->y;
        }

        return Point2D(cx/n,cy/n);
    }

    float PDollarGesture::PathLength()
    {
        float length = 0;
        size_t n = gesturePoints.size();
        for (int i = 1; i< n; i++)
        {
            if(gesturePoints[i]->strokeID == gesturePoints[i-1]->strokeID)
            {
                length += EuclideanDistance(gesturePoints[i-1], gesturePoints[i]);
            }
        }
        return length;
    }

}
