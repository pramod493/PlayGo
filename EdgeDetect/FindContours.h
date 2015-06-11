#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;
/**
 * @brief gbFindContours create  contours from image at given file path
 * @param imgDir
 * @param outerContours
 * @param innerContours
 * @param SaveForDebug
 */
void gbFindContours(string imgDir,
	vector<vector<cv::Point> > &outerContours,
	vector<vector<vector<cv::Point> > > &innerContours,
	bool SaveForDebug);
