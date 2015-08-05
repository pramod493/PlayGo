#include <iostream>
#include "grabcut.h"
#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsScene>
#include <QPixmap>
#include "page.h"
#include <math.h>
#include <algorithm>
#include <QMessageBox>
#include <QDebug>
#include "asmopencv.h"
#include "QsLog.h"
#include "PlayGoController.h"

using namespace cv;

namespace CDI
{
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
			draggin = false;
		}
	}

	int grabcut_load_camera(string outputfile)
	{
		//	// Open Camera
		//	VideoCapture cap(0); // open the default camera
		//	if (!cap.isOpened())  // check if we succeeded
		//		return -1;

		//	// Snap Photo
		//	Mat image;
		//	namedWindow("Camera", 1);
		//	for (;;)
		//	{
		//		Mat frame;
		//		cap >> frame; // get a new frame from camera
		//		imshow("Camera", frame);
		//		if (waitKey(30) >= 0)
		//		{
		//			image = frame;
		//			break;
		//		}
		//	}

		//	return grabcut_process_image(image, outputfile);
		return 0;
	}

	Mat grabCutBackground(cv::Rect rectangle, Mat& image, string outputfile,
						  vector<vector<cv::Point> >& outerContours,
						  vector<vector<vector<cv::Point> > >& innerContours)
	{
		cvtColor(image, image, CV_RGBA2RGB);
		imwrite("Initial.png", image);
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

		vector<vector<cv::Point> > contours;
		vector<cv::Vec4i> hierarchy;

		// Find contours
		cv::findContours(result, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

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

		// Generate output image
		cv::Mat foreground(image.size(), CV_8UC3, cv::Scalar(255, 255, 255));

		image.copyTo(foreground, result); // bg pixels not copied

		// Crop image
		foreground = foreground(rectangle);

		imwrite(outputfile, foreground);

		return foreground;
	}

	int grabcut_process_image(Mat& image, string outputfile)
	{
		// Select Roi

		//	imshow("Camera", image);
		//	for (;;)
		//	{
		//		Mat cln = image.clone();

		//		cvSetMouseCallback("Camera", mouseHandler, NULL);
		//		if (draggin)
		//		{
		//			rectangle(cln, cv::Rect(pt1.x, pt1.y, pt2.x - pt1.x, pt2.y - pt1.y), CV_RGB(255, 0, 0), 3, 8, 0);
		//			imshow("Camera", cln);
		//		}

		//		if (waitKey(30) >= 0)
		//		{
		//			break;
		//		}
		//	}

		//	// Initialization
		//	cv::Rect rectangle(pt1.x, pt1.y, pt2.x-pt1.x, pt2.y-pt1.y);
		//	cv::Mat foreground = grabCutBackground(rectangle, image, outputfile);

		//	// display result
		//	cv::namedWindow("Extracted Image");
		//	cv::imshow("Extracted Image", foreground);



		//	waitKey();
		return 0;

	}

	class grabcutView : public QGraphicsView
	{
	protected:
		QGraphicsRectItem* rectItem;
		PlayGoController * _controller;
		QString _outpath;
		QPointF initPos;
		QPointF finalPos;
	public:
		grabcutView(PlayGoController *controller,
					QString outputPath,
					QWidget *parent)
			: QGraphicsView(parent)
		{
			QGraphicsScene* _scene = new QGraphicsScene(this);
			setScene(_scene);
			rectItem = new  QGraphicsRectItem(QRectF());
			_scene->addItem(rectItem);
			initPos = QPointF();
			finalPos = QPointF();

			_outpath = outputPath;
			_controller = controller;

			Qt::WindowFlags flags = windowFlags();
			setWindowFlags(flags | Qt::WindowStaysOnTopHint);
		}

		void mousePressEvent(QMouseEvent *event)
		{
			rectItem->setZValue(1);
			initPos = mapToScene(event->localPos().toPoint());
			finalPos = initPos;
			updateRect();
		}

		void mouseMoveEvent(QMouseEvent* event)
		{
			finalPos = mapToScene(event->localPos().toPoint());
			updateRect();
		}

		void mouseReleaseEvent(QMouseEvent* event)
		{
			finalPos = mapToScene(event->localPos().toPoint());
			updateRect();

			QRect intRect = rectItem->rect().toRect();
			if (intRect.width()== 0 || intRect.height() == 0)
			{return;}

			QList<QGraphicsItem*> lisItems = items();
			if (lisItems.size() != 2) return;	// Should not have more than 2 items
			QMessageBox::StandardButton reply;
			reply = QMessageBox::question(this,
										  "Image selected",
										  "Process image?",
										  QMessageBox::Yes | QMessageBox::No);
			rectItem->setRect(QRectF());
			if (reply != QMessageBox::Yes)  return;

			for (int i=0; i < lisItems.size(); i++)
			{
				QGraphicsItem* graphicsitem = lisItems[i];
				if (graphicsitem->type() != QGraphicsPixmapItem::Type) continue;

				// Found the item
				// Get bounding rectangle
				QGraphicsPixmapItem* pixmapItem = qgraphicsitem_cast<QGraphicsPixmapItem*>(graphicsitem);
				QRect imageRect = graphicsitem->boundingRect().toRect();
				if (imageRect.contains(intRect))
				{
					vector<vector<cv::Point> > outerContours;
					vector<vector<vector<cv::Point> > > allInnerContours;

					cv::Mat pix = ASM::QPixmapToCvMat(pixmapItem->pixmap(), true);
					cv::Mat croppedmat = grabCutBackground(cv::Rect(intRect.x(), intRect.y(), intRect.width(), intRect.height()),
														   pix, _outpath.toStdString(),
														   outerContours, allInnerContours);
					QPixmap pixmap = QPixmap();
					pixmap.load(_outpath);
					QPixmap tmp = pixmapItem->pixmap();
					pixmapItem->setPixmap(pixmap);
					resize(pixmap.width(), pixmap.height());

					reply = QMessageBox::question(this,
												  "Image processed",
												  "Load image?",
												  QMessageBox::Yes | QMessageBox::No);
					if (reply == QMessageBox::Yes)
					{
						_controller->loadImage(_outpath);
						deleteLater();
					} else
					{
						// Show image edit options
						//					pixmapItem->setPixmap(tmp);
						//					resize(tmp.width(), tmp.height());
					}
				} else
				{
					QMessageBox::about(this, "Invalid selection", "Make sure that selection rectangle is withing image");
				}
			}
		}

		void updateRect()
		{
			float left  = min(initPos.x(), finalPos.x());
			float top = min(initPos.y(), finalPos.y());
			float width = abs(initPos.x() - finalPos.x());
			float height = abs(initPos.y() - finalPos.y());

			rectItem->setRect(QRectF(left, top, width, height));
		}
	};

	int grabcut_process_image(PlayGoController *controller, string imagepath, string outputfile)
	{
		grabcutView *view = new grabcutView(controller, QString::fromStdString(outputfile), NULL);
		view->show();
		QPixmap pixmap = QPixmap();
		pixmap.load(QString::fromStdString(imagepath));
		pixmap = pixmap.scaled(900,900, Qt::KeepAspectRatio);
		QGraphicsPixmapItem* pixmapItem = view->scene()->addPixmap(pixmap);
		view->resize(pixmap.width(), pixmap.height());
		return 1;
	}
}
