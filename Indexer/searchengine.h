
/*
 * Developer : William J Benjamin (benjamin.william@gmail.com)
 * [C]Design Lab, Purdue University, USA
 * Date : Oct 2012
 */
#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H



//#include "ShapeData.h"

#include <vector>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include "parallelbice.h"
#include "tbb/tbb.h"

using namespace cv;
using namespace boost::filesystem;

class Shape;
class cv::gpu::GpuMat;
class SiftGPU;
class SearchEngine{



public:
  SearchEngine(char* c_detect,char * c_extract, char * c_match);

  // Given a folder Create Dictionary & Compute BOW descriptors
  void StartEngine(const path& basepath);

  // Read extract all the features from the folder
  void CreateDictionary(const path& basepath);

  // Perform culling based on size & response and resample on a grid.
  void RefineKeyPoints(vector<KeyPoint> &keypoints,vector<KeyPoint> &refined_keypoints);

  // Compute the BOW descriptors for each image
  void ComputeBOWDescriptor(const path& basepath, Mat& descriptors,vector<std::string> &files);

  // Extract Descriptors from Query
  void Query(const Mat &img,int k, Mat &dists,Mat & results,vector<std::string> &files);
 //void Query2(const Mat &img2,int k, Mat &dists,Mat & results,vector<std::string> &ifiles);


  void ComputeMatches(const Mat &img, const path &basepath);

  void ComputeDescriptors(const path& basepath);
  void ComputeDistanceMatrix(const path& query_path, const path& basepath);




  void Init();
  void Del();


  // Pixel Firing
  void StartPFEngine(const path& basepath);
  void StartBICEngine(const path& basepath);
  void LoadBICEngine(const path& basepath);
  //void QueryPFEngine(const Mat &img,int k, Mat &dists,Mat & results,vector<std::string> &files);

  vector<Shape*>  QueryPFEngine(Mat *img);
  vector<Shape*>  QueryBICEngine(Mat &img);

  Mat GetBICEDescriptor(Mat &img);
   Mat GetBICEDescriptorParallel(Mat &img);
  void DrawGrid(int &numx_cell,int &numy_cell, Mat &img);
  int ClassifyAngle(double &angle, int &num_classes);
  int Bin(double &val,double &ll,double &ul,int &num_classes);
  Mat ComputeBICEAccumPatchDescriptor(vector<Mat> &accum_vec,int &num_theta_classes,int &num_bin_x,int & num_bin_y,double &tau);
  Mat ComputeBICEAccumPatchDescriptor2(vector<Mat> &accum_vec,int &num_theta_classes,int &num_bin_x,int & num_bin_y,double &tau);

private:


  //Ptr<DescriptorMatcher> matcher ;
  //Ptr<SiftDescriptorExtractor> extractor ;
  //Ptr<FeatureDetector> detector ;
  //FeatureDetector * detector;
  //DynamicAdaptedFeatureDetector *detector;
  DescriptorExtractor * extractor;
  DescriptorMatcher * matcher;

  //cv::SIFT *extractor;

  cv::FeatureDetector * detector;

  BOWKMeansTrainer *bowTrainer;
  BOWImgDescriptorExtractor *bowDE;
  flann::Index *flannIndex;
  vector<std::string> files;

  CvSize sz;

  int dictionarySize ;
  TermCriteria tc;
  int retries;
  int flags;

  vector<cv::gpu::GpuMat> dbimages;
  vector<Shape*> m_shapes;
  vector<cv::Mat> m_descriptors;
  SiftGPU  *m_sift ;
  Mat m_trainingData;


};

class Shape
{

public:
    Shape(string &str_name)
    {
        m_file_path = str_name;
        m_sim = 0;

    }
    static bool compareit(Shape *a, Shape *b)
    {
        return a->m_sim > b->m_sim;

    }

public:
double m_sim;
string m_file_path;


};





#endif // SEARCHENGINE_H
