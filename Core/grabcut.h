#pragma once
#include <QFile>
#include <QDir>
#include <QPixmap>
#include <QBitmap>
#include <QDebug>
#include <QString>
#include <QStringList>
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace std;

namespace CDI
{
	class PlayGoController;

	cv::Mat grabCutBackground(cv::Rect rectangle, cv::Mat& image, string outputfile, vector<vector<cv::Point> > &outerContours, vector<vector<vector<cv::Point> > > &innerContours);

	int grabcut_process_image(PlayGoController* controller, string imagepath, string outputfile);
}
