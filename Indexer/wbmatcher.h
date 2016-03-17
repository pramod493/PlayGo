#ifndef WBMATCHER_H
#define WBMATCHER_H
#include <opencv2/opencv.hpp>

class wbmatcher
{
public:
    wbmatcher();
    virtual double ComputeDistance(cv::Mat &query,cv::Mat &data );
};

#endif // WBMATCHER_H
