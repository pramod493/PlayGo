#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include "tbb/tbb.h"
#include "tbb/parallel_for.h"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#define TBB_USE_DEBUG 1
using namespace std;
using namespace boost::filesystem;
using namespace cv;

int main(int argc, char** argv)
{
    namespace po = boost::program_options;
    std::cout << "Running code\n";

    path data("E:/Indexer-WIP");

    tbb::task_scheduler_init init;
    init.terminate();
    std::cout << "success in threading... \n";

    cv::Mat image;

    imread("sobel.png", CV_LOAD_IMAGE_COLOR);

    std::cout << "Initialized opencv matrix\n";

};
