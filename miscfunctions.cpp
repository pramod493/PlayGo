#include <QFile>
#include <QDir>
#include <QPixmap>
#include <QBitmap>
#include <QDebug>
#include <QString>
#include <QStringList>
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;


cv::Point pt1, pt2;
bool draggin = false;
void mouseHandler(int event, int x, int y, int flags, void* param)
{
	if (event == CV_EVENT_LBUTTONDOWN)
	{
		pt1 = Point(x, y);
		draggin = true;
	}
	if (event == CV_EVENT_MOUSEMOVE && draggin )
	{
		pt2 = Point(x, y);
	}
	if (event == CV_EVENT_LBUTTONUP)
	{
		pt2 = Point(x, y);
		//draggin = false;
	}
}

int grabcut_load(string outputfile)
{
	// Open Camera
	/*VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
		return -1;

	// Snap Photo
	Mat image;
	namedWindow("Camera", 1);
	for (;;)
	{
		Mat frame;
		cap >> frame; // get a new frame from camera
		imshow("Camera", frame);
		if (waitKey(30) >= 0)
		{
			image = frame;
			break;
		}
	}

	// Select Roi

	imshow("Camera", image);
	for (;;)
	{
		Mat cln = image.clone();

		cvSetMouseCallback("Camera", mouseHandler, NULL);
		if (draggin)
		{
			rectangle(cln, cv::Rect(pt1.x, pt1.y, pt2.x - pt1.x, pt2.y - pt1.y), CV_RGB(255, 0, 0), 3, 8, 0);
			imshow("Camera", cln);
		}

		if (waitKey(30) >= 0)
		{
			break;
		}
	}

	// Initialization
	cv::Rect rectangle(pt1.x, pt1.y, pt2.x-pt1.x, pt2.y-pt1.y);
	cv::Mat result; // segmentation result (4 possible values)
	cv::Mat bgModel, fgModel; // the models (internally used)

	// GrabCut segmentation
	cv::grabCut(image,    // input image
		result,   // segmentation result
		rectangle,// rectangle containing foreground
		bgModel, fgModel, // models
		1,        // number of iterations
		cv::GC_INIT_WITH_RECT); // use rectangle
	// Get the pixels marked as likely foreground
	cv::compare(result, cv::GC_PR_FGD, result, cv::CMP_EQ);

	// Generate output image
	cv::Mat foreground(image.size(), CV_8UC3, cv::Scalar(255, 255, 255));

	image.copyTo(foreground, result); // bg pixels not copied

	// display result
	cv::namedWindow("Extracted Image");
	cv::imshow("Extracted Image", foreground);

	{
//		strcat(argv[1], ".png");
//		std::cout << argv[1];
		imwrite(outputfile, foreground);
	}

	waitKey();*/
	return 0;

}
/**
 * @brief GenerateMaskFromFile generates mask from given file
 * based on alpha channel values
 * @param sourceFile
 * @param destFile
 * @remark No sanity check are done
 */
void GenerateMaskFromFile(QString sourceFile, QString destFile)
{
	QPixmap pixmap = QPixmap();
	pixmap.load(sourceFile);
	QBitmap mask = pixmap.mask();
	mask.save(destFile);
}

/**
 * @brief GenerateMask Generate mask based on alpha channel for all files in directory
 * @param sourceDir: Source file directory
 * @param destDir: Output file directory (NOTE: Will overwrite
 *					existing file in case same as sourceDir)
 * @return false when either of the directories do not exist and output
 * directory cannot be created
 *
 * Does not perform any checks on file type. Also that a black image will be written
 * when alpha channel is missing
 */
bool GenerateMask(QString sourceDir, QString destDir)
{
	QDir sdir(sourceDir);
	if (!sdir.exists()) return false;
	QDir outdir(destDir);
	if (!outdir.exists())
		if (!outdir.mkpath(destDir))
			return false;

	QStringList fileList = sdir.entryList(QDir::Files);

	QStringList::const_iterator iter;
	for (iter = fileList.constBegin(); iter != fileList.constEnd();
		 ++iter)
	{
		QString name = (*iter);
		QString fullpath = sdir.filePath(name);
		// If both sourceDir and outDir are same, then it will overwrite
		QString fullOutPath = outdir.filePath(name);
		GenerateMaskFromFile(fullpath, fullOutPath);
	}
	return true;
}

/*
 * Mask details:
 * turn on a mask
 * mask |= MASK_BIT3
 * Check mask
 * mask & MASK_BT3
 * Reset mask
 * mask &= ~MASK_BIT3
 * */
