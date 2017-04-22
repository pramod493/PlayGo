#pragma once
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <vector>
#include <iostream>
#include <string>

using namespace std;
/**
 * @brief gbFindContours create  contours from image at given file path
 * @param imgDir Image path
 * @param outerContours Contains outer contour
 * @param innerContours Contains inner contour
 * @param SaveForDebug Saves intermediate files in the working directory
 */
void gbFindContours(string imgDir,
	vector<vector<cv::Point> > &outerContours,
	vector<vector<vector<cv::Point> > > &innerContours,
    bool SaveForDebug = false);
