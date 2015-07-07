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
#include "PlayGoController.h"

using namespace cv;
using namespace std;
using namespace CDI;

void mouseHandler(int event, int x, int y, int flags, void* param);

int grabcut_load_camera(string outputfile);

Mat grabCutBackground(cv::Rect rectangle, Mat& image, string outputfile, vector<vector<Point> > &outerContours, vector<vector<vector<Point> > > &innerContours);

int grabcut_process_image(Mat& image, string outputfile);

int grabcut_process_image(PlayGoController* controller, string imagepath, string outputfile);
