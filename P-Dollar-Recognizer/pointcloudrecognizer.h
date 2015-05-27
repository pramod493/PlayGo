#pragma once

#include <string>
#include <vector>
#include "pdollargesture.h"
#include "gesturepoint2d.h"

using namespace std;

namespace CDI
{
    class PointCloudRecognizer
    {
    public:
        string Classify(PDollarGesture* candidate, vector<PDollarGesture*> &trainingSet);

    private:
        float GreedyCloudMatch(vector<GesturePoint2D*> &points1,
                               vector<GesturePoint2D*> &points2);

        float CloudDistance(vector<GesturePoint2D*> &points1,
                            vector<GesturePoint2D*> &points2,
                            int startIndex);
    };
}
