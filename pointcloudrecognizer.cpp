#include "pointcloudrecognizer.h"

namespace CDI
{
    string PointCloudRecognizer::Classify(PDollarGesture* candidate,
                                          vector<PDollarGesture*> &trainingSet)
    {
        float minDistance = CDI_MAX_FLOAT;
        string gestureClass = "";

        for (int i=0; i<trainingSet.size(); i++)
        {
            PDollarGesture* templateGesture = trainingSet[i];
            float dist = GreedyCloudMatch(candidate->gesturePoints, templateGesture->gesturePoints);
            if (dist < minDistance)
            {
                minDistance = dist;
                gestureClass = templateGesture->gestureName;
            }
        }
        return gestureClass;
    }



    float PointCloudRecognizer::GreedyCloudMatch(vector<GesturePoint2D*> &points1,
                                                 vector<GesturePoint2D*> &points2)
    {
        int n = points1.size();
        float eps = 0.05f;
        int step = floor(pow(n, 1.0-eps));

        float minDistance = CDI_MAX_FLOAT;
        for(int i=0;i<n;i+=step)
        {
            float dist1 = CloudDistance(points1, points2, i);
            float dist2 = CloudDistance(points2, points1, i);
            minDistance = min(minDistance, min(dist1,dist2));
        }
        return minDistance;
    }

    float PointCloudRecognizer::CloudDistance(vector<GesturePoint2D *> &points1,
                                              vector<GesturePoint2D *> &points2,
                                              int startIndex)
    {
        int n = points1.size();
        bool* matched = new bool[n];
        // Initialize each element to false
        for(int i=0;i<n;i++) matched[i] = false;

        float sum = 0;
        int i = startIndex;
        do
        {
            int index = -1;
            float minDistance = CDI_MAX_FLOAT;
            for (int j=0; j<n; j++)
            {
                if (!matched[j])
                {
                    float dist = SqrEuclideanDistance(points1[i], points2[j]);
                    if (dist < minDistance)
                    {
                        minDistance = dist;
                        index = j;
                    }
                }
            }
            matched[index] = true; // point index from the 2nd cloud is matched to point i from the 1st cloud
            float weight = 1.0f - ((i - startIndex + n) % n) / (1.0f * n);
            sum += weight * minDistance; // weight each distance with a confidence coefficient that decreases from 1 to 0
            i = (i + 1) % n;
        } while(i != startIndex);
        return sum;
    }
}
