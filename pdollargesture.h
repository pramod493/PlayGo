#pragma once
#include <algorithm>
#include <vector>
#include "string"
#include "gesturepoint2d.h"
#include "gesturestroke.h"
#include "cdicommon.h"


using namespace std;

namespace CDI
{
    class PDollarGesture
    {
    public:
        vector<GesturePoint2D*> gesturePoints;

        string gestureName;

        int SAMPLING_RESOLUTION;

        PDollarGesture(vector<GestureStroke*> strokes, string name = "");

        PDollarGesture(vector<GesturePoint2D*> points, string name = "");

        ~PDollarGesture();

    protected:
        void Scale();

        void TranslateTo(Point2D);

        void Resample();

        Point2D Centroid();

        float PathLength();

    };
}
