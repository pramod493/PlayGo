#include "FindContours.h"

void gbFindContours(string imgDir,
	vector<vector<cv::Point> >& outerContours,
	vector<vector<vector<cv::Point> > >& innerContours,
	bool SaveForDebug )
{

	SaveForDebug = false;
	outerContours.clear();
	innerContours.clear();

	// Load source image with alpha channel
	/* 
	If want to pass in the image instead, make sure the 
	passed in contains the alpha channel, in opencv, the 
	negative flag of imread will do this
	*/
	cv::Mat src = cv::imread(imgDir, -1);

	/* For Testing */
	if (SaveForDebug){
		std::cout << "number of channels are : " << src.channels()<<std::endl;
	}
	/* For Testing */
	
	//  Split channels, Alpha channel will be channel[3]
	// if it's bw image, the mask could be itself

	cv::Mat mask = cv::Mat::zeros(src.rows, src.cols, CV_8UC1);

	if (src.channels() == 4)
	{
		// We should find mask through alpha channel

		cv::Mat channel[4];
		cv::split(src, channel);

		// Mask on the alpha channel
		for (unsigned r = 0; r < src.rows; r++){
			for (unsigned c = 0; c < src.cols; c++){
				if (channel[3].at<uchar>(r, c) != 0){
					// if it's not transparent, alpha > 0
					// there's a kinda reversal here
					mask.at<uchar>(r, c) = 255;
				}
			}
		}
	}
	else if (src.channels() == 1)
	{
		cv::threshold(src, mask, 200, 255, THRESH_BINARY_INV);
	}
	else
	{
		// Now it's probably 3 channel, we need special treatment
		// Let's not consider them right nnow
	}
		


	// Remove small holes, optional but preferred
	// Only remove 1 pixel
	cv::dilate(mask, mask, Mat(), Point(-1, -1), 1);
	cv::erode(mask, mask, Mat(), Point(-1, -1), 1);

	/* For Testing */
	if (SaveForDebug){
		imwrite("mask.png", mask);
	}
	/* For Testing */

	// Since we have the BW image already, canny search is not required
	// We would just use the findContours function
	vector<vector<cv::Point> > contours;
	vector<cv::Vec4i> hierarchy;

	// Find contours
	cv::findContours(mask, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	// Store them as required
	// in hierarchy, the order is : 
	// 0 next, 1 previous, 2 first child, 3 parent
	// negative if none exist

	vector<vector<cv::Point> > temp;

	int outerIndex = 0;
	int innerIndex = 0;
	while (outerIndex >= 0)
	{
		outerContours.push_back(contours[outerIndex]);

		// Store it's inner holes

		/*
		// IMPORTANT, WE WORKING WITH THE CASE OF TWO LAYER HIERACHY ONLY HERE
		*/

		temp.clear();
		innerIndex = hierarchy[outerIndex][2];
		while (innerIndex >= 0)
		{
			// if there is no hole
			// the while loop won't be executed
			// and hence temp will be empty

			temp.push_back(contours[innerIndex]);

			// next inner hole
			innerIndex = hierarchy[innerIndex][0];
		}
		innerContours.push_back(temp);

		/*
		// IMPORTANT, WE WORKING WITH THE CASE OF TWO LAYER HIERACHY ONLY HERE
		*/

		outerIndex = hierarchy[outerIndex][0];	// next outer contour
	}

	/* For Testing */
	if (SaveForDebug){
		std::cout << "number of outer contours are : " << outerContours.size() << std::endl;
		int sum = 0;
		for (int i = 0; i < innerContours.size(); i++){
			sum += innerContours[i].size();
		}
		std::cout << "number of inner contours are : " << sum << std::endl;
	}

	if (SaveForDebug){
		cv::RNG rng(12345);
		Mat drawing = Mat::zeros(mask.size(), CV_8UC3);
		for (int i = 0; i< contours.size(); i++)
		{
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
		}
		imwrite("Contours.png", drawing);
	}
	/* For Testing */
}
