//#include "ShapeData.h"
#include <vector>
#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/gpu/gpumat.hpp>
#include <opencv2/gpu/gpu.hpp>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "searchengine.h"
#include <numeric>
#include "tbb/parallel_for.h"
#include "SiftGPU/src/SiftGPU/SiftGPU.h"
#include <QDebug>


using namespace std;
using namespace boost::filesystem;
using namespace cv;

//#define MAX_NUM_THETA_BINS 6

SearchEngine::SearchEngine(char* c_detect,char * c_extract, char * c_match)
{
    // SIFT Feature Parameters
    int nfeatures=0;
    int nOctaveLayers=3;
    double contrastThreshold=0.01;
    double edgeThreshold=20;
    double sigma=1.6;
    extractor = new cv::SIFT(nfeatures,nOctaveLayers,contrastThreshold,edgeThreshold,sigma);

    cv::SiftFeatureDetector * siftdetect = new cv::SiftFeatureDetector( contrastThreshold, edgeThreshold);
    detector = new cv::GridAdaptedFeatureDetector(new cv::SurfAdjuster(5.0, true),1200,10,10);

    matcher = new cv::FlannBasedMatcher();
    dictionarySize = 1000;
    TermCriteria tc(CV_TERMCRIT_ITER, 10, 0.001);
    retries = 1;
    flags = KMEANS_PP_CENTERS;
    bowTrainer = new BOWKMeansTrainer(dictionarySize, tc, retries, flags);
    //See article on BoW model for details
    bowDE = new BOWImgDescriptorExtractor(extractor, matcher);
}

void SearchEngine::Init()
{
    detector = new DynamicAdaptedFeatureDetector(new SurfAdjuster(10.0,2,100),100,150,5);
    extractor = new cv::SIFT();
    matcher = new cv::FlannBasedMatcher();
    dictionarySize = 256;
    TermCriteria tc(CV_TERMCRIT_ITER, 10, 0.001);
    retries = 1;
    flags = KMEANS_PP_CENTERS;
    bowTrainer = new BOWKMeansTrainer(dictionarySize, tc, retries, flags);
    bowDE = new BOWImgDescriptorExtractor(extractor, matcher);
}

void SearchEngine::Del()
{
    if(detector!=NULL)
    {
        delete detector;
        detector = NULL;
    }
    if(extractor)
    {
        delete extractor;
        extractor = NULL;
    }
    if(matcher!=NULL)
    {
        delete matcher;
        matcher = NULL;
    }
}


void SearchEngine::CreateDictionary(const path& basepath)
{
    for (directory_iterator iter = directory_iterator(basepath); iter
         != directory_iterator(); iter++) {
        directory_entry entry = *iter;

        if (is_directory(entry.path())) {
            cout << "Processing directory " << entry.path().string() << endl;
            CreateDictionary(entry.path());
        } else {
            path entryPath = entry.path();
            if (entryPath.extension() == ".png") {

                cout << "Processing file " << entryPath.string() << endl;
                Mat img = imread(entryPath.string(),0);
                if (!img.empty()) {
                    vector<KeyPoint> keypoints;
                    detector->detect(img, keypoints);
                    if (keypoints.empty()) {
                        cerr << "Warning: Could not find key points in image: "
                             << entryPath.string() << endl;
                    } else {
                        Mat features;
                        Mat img32F;
                        //img.convertTo(img32F,CV_32F);
                        int etype = extractor->descriptorType();
                        //extractor->detect(img,keypoints);
                        extractor->compute(img, keypoints, features);

                        // save the feature descriptors


                        cout<< "Number of keypoints found:"<< keypoints.size()<<endl;
                        //cout << "M = "<< endl << " "  << features << endl << endl;
                        bowTrainer->add(features);
                    }
                } else {
                    cerr << "Warning: Could not read image: "
                         << entryPath.string() << endl;
                }

            }
        }
    }
}


void SearchEngine::ComputeDescriptors(const path& basepath)
{
    for (directory_iterator iter = directory_iterator(basepath); iter
         != directory_iterator(); iter++) {
        directory_entry entry = *iter;

        if (is_directory(entry.path())) {

            cout << "Processing directory " << entry.path().string() << endl;
            CreateDictionary(entry.path());

        } else {


            path entryPath = entry.path();


            if (entryPath.extension() == ".png") {

                cout << "Processing file " << entryPath.string() << endl;
                Mat img = imread(entryPath.string(),0);
                if (!img.empty()) {
                    vector<KeyPoint> keypoints;
                    detector->detect(img, keypoints);
                    if (keypoints.empty()) {
                        cerr << "Warning: Could not find key points in image: "
                             << entryPath.string() << endl;
                    } else {
                        Mat features;
                        Mat img32F;
                        //img.convertTo(img32F,CV_32F);
                        int etype = extractor->descriptorType();
                        //extractor->detect(img,keypoints);
                        extractor->compute(img, keypoints, features);

                        path desc_file = entryPath.replace_extension(".yml");
                        FileStorage fs(desc_file.string(), FileStorage::WRITE);
                        //fs<<"keypoints"<<keypoints;
                        fs << "features"<<features;
                        fs.release();



                        // save the feature descriptors


                        cout<< "Number of keypoints found:"<< keypoints.size()<<endl;

                        //bowTrainer->add(features);
                    }
                } else {
                    cerr << "Warning: Could not read image: "
                         << entryPath.string() << endl;
                }

            }
        }
    }
}

void SearchEngine::ComputeDistanceMatrix(const path& query_path, const path& basepath)
{
    Mat query_features;

    FileStorage fs1(query_path.string(), FileStorage::READ);
    Mat features_query;

    fs1["features"]>>features_query;
    fs1.release();
    for (directory_iterator iter = directory_iterator(basepath); iter
         != directory_iterator(); iter++) {
        directory_entry entry = *iter;
        path entryPath = entry.path();


        if (entryPath.extension() == ".yml") {

            cout << "Processing file " << entryPath.string() << endl;
            FileStorage fs(entryPath.string(), FileStorage::READ);
            Mat features;

            fs["features"]>>features;
            fs.release();

            cv::BFMatcher matcher( cv::NORM_L2, false );

            // Match the two image descriptors
            std::vector<cv::DMatch> matches;
            matcher.match(features_query,features, matches);

            double min_dist =10000;
            double max_dist =0;
            for( int i = 0; i < features_query.rows; i++ )
            { double dist = matches[i].distance;
                if( dist < min_dist ) min_dist = dist;
                if( dist > max_dist ) max_dist = dist;
            }

            std::vector< DMatch > good_matches;

            for( int i = 0; i < features_query.rows; i++ )
            { if( matches[i].distance < 1.5*min_dist )
                { good_matches.push_back( matches[i]); }
            }
            double avg = (features_query.rows + features.rows)/2;

            cout<< "Score"<< good_matches.size()/avg<<endl;
        }

    }
}

void SearchEngine::RefineKeyPoints(vector<KeyPoint> &keypoints,vector<KeyPoint> &refined_keypoints)
{

}

void SearchEngine::ComputeBOWDescriptor(const path& basepath, Mat& descriptors, vector<std::string> &ifiles) {
    for (directory_iterator iter = directory_iterator(basepath); iter
         != directory_iterator(); iter++) {
        directory_entry entry = *iter;
        if (is_directory(entry.path())) {
            cout << "Processing directory " << entry.path().string() << endl;
            ComputeBOWDescriptor(entry.path(), descriptors, files);
        } else {
            path entryPath = entry.path();
            if (entryPath.extension() == ".png") {

                cout << "Processing file " << entryPath.string() << endl;
                Mat img = imread(entryPath.string(),0);
                if (!img.empty()) {
                    vector<KeyPoint> keypoints;
                    detector->detect(img, keypoints);
                    if (keypoints.empty()) {
                        cerr << "Warning: Could not find key points in image: "
                             << entryPath.string() << endl;
                    } else {
                        Mat bowDescriptor;
                        //cout<<"Works until Line Number " <<__LINE__<<endl;
                        int dtype = bowDE->descriptorType();
                        //cout<<"Descriptor type is "<<dtype<<endl;
                        bowDE->compute(img, keypoints, bowDescriptor);
                        //cout << "M = "<< endl << " "  << bowDescriptor << endl << endl;
                        descriptors.push_back(bowDescriptor);
                        ifiles.push_back(entry.path().string());


                        // save the descriptor for the file.
                        boost::filesystem::path dir = entryPath.parent_path();
                        boost::filesystem::path desc_dir = dir / "sift";

                        if(!exists( desc_dir ))
                        {
                            create_directory(desc_dir);
                        }

                        std::string file_base= entryPath.stem().string();
                        file_base.append(".yml");

                        boost::filesystem::path desc_file = desc_dir / file_base;

                        FileStorage fs(desc_file.string(), FileStorage::WRITE);
                        fs << "siftdesc" <<bowDescriptor;
                        fs.release();
                    }
                } else {
                    cerr << "Warning: Could not read image: "
                         << entryPath.string() << endl;
                }
            }
        }
    }
}


void ComputeDescriptors(const path &basepath)
{
    // compute keypoints and descriptors for the db and save it.
}

void ComputeMatches(const path &basepath)
{

}
void SearchEngine::StartEngine(const path& basepath)
{
    cout<<"Creating Dictionary"<<endl;
    CreateDictionary(basepath);

    int count=bowTrainer->descripotorsCount();

    cout<<"Clustering "<<count<<" features"<<endl;

    vector<Mat> descriptors = bowTrainer->getDescriptors();

    cout << "Type is "<< descriptors[1].depth()<<endl;

    int descCount = 0;
    for( size_t i = 0; i < descriptors.size(); i++ )
        descCount += descriptors[i].rows;
    Mat dictionary = bowTrainer->cluster();

    //cout << "dictionary= "<< endl << " "  << dictionary<< endl << endl;

    cout<<"Created Dictionary from "<<count<<" features"<<endl;
    bowDE->setVocabulary(dictionary);
    cout<<"Generating Bag of Word Descriptors"<<endl;
    m_trainingData= Mat(0, dictionarySize, CV_32FC1);
    //Mat labels(0, 1, CV_32FC1);

    vector<std::string> ifiles;
    ComputeBOWDescriptor(basepath, m_trainingData, ifiles);
    cout<<"Generated Bag of Word Descriptors"<<endl;

    cout << "trainingData= "<< endl << " "  << m_trainingData<< endl << endl;

    files = ifiles;
    //cout << "M = "<< endl << " "  << labels[1] << endl << endl;

    // Processing Query
    cout<<"Computing the FLANN index"<<endl;

    FileStorage fs("test.yml", FileStorage::WRITE);
    sz = m_trainingData.size();
    fs << "height" << sz.height;
    fs << "width" <<sz.width;
    fs << "dictionary"<<dictionary;
    fs<< "files"<< files;
    fs.release();
}


void SearchEngine::Query(const Mat &img2,int k, Mat &dists,Mat & results,vector<std::string> &ifiles)
{
    // Compute the descriptor for the query
    if (img2.empty())
    {   cerr<<"Warning: Could not read query file"<<endl;
        return;
    }

    // Mat trgdata = cvLoad("trainingdata");
    //FileStorage fs(filename,FileStorage::READ);

    cv::FileStorage fs("test.yml", FileStorage::READ );
    int height;
    int width;
    Mat dictionary;
    vector<std::string> files;
    fs["height"]>> height;
    fs["width"]>> width;
    fs["dictionary"]>>dictionary;
    fs["files"]>>files;

    //CvSize ss;
    cv::Mat indexMat(cvSize(width,height), CV_32FC1);

    Mat query = Mat::zeros(1,dictionarySize,CV_32FC1);
    Mat img;
    cvtColor(img2, img, CV_RGB2GRAY);
    vector<KeyPoint> keypoints;
    detector->detect(img, keypoints);
    if (keypoints.empty()) {
        cerr << "Warning: Could not find key points in image: "<< endl;
        return;
    } else {

        int dtype = bowDE->descriptorType();
        bowDE->compute(img, keypoints, query);

    }


    // KdTree with 5 random trees
    cv::flann::KDTreeIndexParams indexParams(5);

    // You can also use LinearIndex
    //cv::flann::LinearIndexParams indexParams;

    // Create the Index
    cv::flann::Index kdtree(m_trainingData, indexParams);

    cout << "Performing single search to find closest data point to mean:" << endl;
    //vector<float> singleQuery;
    vector<int> index(3);
    vector<float> dist(3);



    // Invoke the function
    kdtree.knnSearch(query, index, dist,3, cv::flann::SearchParams(64));

    // Print single search results
    cout << "(index,dist):" << index[1] << "," << dist[1]<< endl;




    ifiles = files;

    cout << "dists= "<< endl << " "  << query<< endl << endl;
    cout << "indices= "<< endl << " "  << results<< endl << endl;
    //flannIndex->buildIndex();
    FileStorage fs2("results.yml", FileStorage::WRITE);
    //Mat cameraMatrix = (Mat_<double>(3,3) << 1000, 0, 320, 0, 1000, 240, 0, 0, 1);
    fs2 << "results" << results;
    fs2.release();

    cerr<<"Done computing FLANN"<<endl;
    //delete flannIndex;
    //Del();
}


void SearchEngine::StartBICEngine(const path& basepath)
{
    for (directory_iterator iter = directory_iterator(basepath); iter
         != directory_iterator(); iter++) {
        directory_entry entry = *iter;

        if (is_directory(entry.path())) {

            cout << "Processing directory " << entry.path().string() << endl;
            StartBICEngine(entry.path());

        } else {

            path entryPath = entry.path();
            if (entryPath.extension() == ".png") {

                cout << "Processing Pixel Fire DB file " << entryPath.string() << endl;
                Mat img = imread(entryPath.string(),0);
                if (!img.empty()) {

                    // path data_filename = entryPath.replace_extension(".yml");
                    // Mat descriptor = GetBICEDescriptor(img);
                    Mat descriptor = GetBICEDescriptorParallel(img);
                    //m_descriptors.push_back(descriptor); // save images
                    //files.push_back(entry.path().string()); // save the filenames



                    // save the descriptor for the file.
                    boost::filesystem::path dir = entryPath.parent_path();
                    boost::filesystem::path desc_dir = dir / "bice";

                    if(!exists( desc_dir ))
                    {
                        create_directory(desc_dir);
                    }

                    std::string file_base= entryPath.stem().string();
                    file_base.append(".yml");

                    boost::filesystem::path desc_file = desc_dir / file_base;

                    std::string str = entry.path().string();

                    FileStorage fs(desc_file.string(), FileStorage::WRITE);
                    fs << "filepath"<<str;
                    fs<< "descriptor"<< descriptor;
                    fs.release();


                } else {
                    cerr << "Warning: Could not read image: "
                         << entryPath.string() << endl;
                }

            }
        }
    }
}


void SearchEngine::LoadBICEngine(const path& basepath)
{
    for (directory_iterator iter = directory_iterator(basepath); iter
         != directory_iterator(); iter++)
    {
        directory_entry entry = *iter;

        if (is_directory(entry.path()))
        {       cout << "Processing directory " << entry.path().string() << endl;
            LoadBICEngine(entry.path());
        } else
        {
            path entryPath = entry.path();
            if (entryPath.extension() == ".yml")
            {

                cout << "Loading file" << entryPath.string() << endl;
                cv::FileStorage fs(entryPath.string(), FileStorage::READ );
                // extract data from files
                Mat descriptor;
                std::string str;
                fs["filepath"]>>str;
                fs["descriptor"]>>descriptor;

                m_descriptors.push_back(descriptor); // save images
                files.push_back(str); // save the filenames

                Shape * p_shape = new Shape(str);
                m_shapes.push_back(p_shape);

                fs.release();
            } else
            {
                cerr << "Warning: Could not read image: "<< entryPath.string() << endl;
            }
        }

    }
}

vector<Shape*> SearchEngine::QueryBICEngine(Mat &img2)
{

    double tin = (double)getTickCount();
    // QUERY PROCESSING

    Mat img = imread("sketch.jpg",0);
    resize(img,img, Size(300,300));
    Mat query_desc = GetBICEDescriptor(img);

    Scalar sum_query = cv::sum(query_desc);

    //    FileStorage fs("query.yml", FileStorage::WRITE);
    //    fs<< "descriptor"<< query_desc;
    //    fs.release();

    std::vector<double> similarity;

    Mat mul_mat;
    typedef std::pair<int,double> mypair;
    std::vector<mypair> simpairs;
    // Compute responses
    int count = 0;

    double t_comp = (double)getTickCount();

    for (std::vector<cv::Mat>::iterator it = m_descriptors.begin() ; it != m_descriptors.end(); ++it)
    {
        Mat db_img_desc =  *it;
        cv::bitwise_and(db_img_desc,query_desc,mul_mat);
        Scalar val = cv::sum(mul_mat);
        //Scalar val =  sum(mul_mat_32fc1);
        double d_val = val(0);
        similarity.push_back(d_val);
        Shape * p_shape = m_shapes[count];
        p_shape->m_sim = d_val;
        count++;
    }

    vector<Shape*> new_shapes(m_shapes);
    std::sort(new_shapes.begin(), new_shapes.end(), Shape::compareit);
    //std::vector<Shape*>::iterator it2;

    vector<Shape*> ret_vec;
    for (std::vector<Shape*>::iterator it2 = new_shapes.begin() ; it2 != new_shapes.begin()+28; ++it2)
    {
        ret_vec.push_back(*it2);
    }

    // std::copy ( new_shapes.begin(), new_shapes.begin()+20, ret_vec.begin() );
    double t_fin = (double)getTickCount();
    cout<<"Total time for retrieval t = "<<(t_fin - tin)/getTickFrequency()<< " secs"<<endl;
    cout<<"Time for descriptor computation t_comp = "<<(t_comp - tin)/getTickFrequency()<< " secs"<<endl;
    cout<<"Time for querying t_comp = "<<(t_fin - t_comp)/getTickFrequency()<< " secs"<<endl;
    return ret_vec;
}

void SearchEngine::StartPFEngine(const path& basepath)
{
    for (directory_iterator iter = directory_iterator(basepath); iter
         != directory_iterator(); iter++) {
        directory_entry entry = *iter;

        if (is_directory(entry.path())) {

            cout << "Processing directory " << entry.path().string() << endl;
            StartPFEngine(entry.path());

        } else {

            path entryPath = entry.path();
            if (entryPath.extension() == ".png") {

                cout << "Processing Pixel Fire DB file " << entryPath.string() << endl;
                Mat img = imread(entryPath.string(),0);
                if (!img.empty()) {

                    Mat descriptor = GetBICEDescriptor(img);


                    Mat img_32f_gray,img_32f_gray_blur;

                    img.convertTo(img_32f_gray,CV_32FC1);

                    int ftype = img_32f_gray.type();

                    img_32f_gray_blur = img_32f_gray.clone();
                    blur( img_32f_gray, img_32f_gray_blur, cv::Size( 20,20 ), cv::Point(-1,-1) );
                    cv::imwrite("gray.jpg", img_32f_gray_blur);

                    Mat db_img_32f,db_img_8uf,db_img_8uf_thres;
                    img_32f_gray_blur.convertTo(db_img_8uf,CV_8UC1);
                    adaptiveThreshold(db_img_8uf, db_img_8uf_thres,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY_INV,31,2);
                    int ftype2 = db_img_8uf_thres.type();
                    cv::imwrite("gray.jpg", db_img_8uf_thres);

                    cv::gpu::GpuMat gpu_mat_db_img;
                    gpu_mat_db_img.upload(db_img_8uf_thres);

                    dbimages.push_back(gpu_mat_db_img); // save images
                    files.push_back(entry.path().string()); // save the filenames

                    //ShapeData * shape = new ShapeData(entry.path());
                    // m_shapes.push_back(shape);
                    std::string str = entry.path().string();
                    Shape * p_shape = new Shape(str);
                    m_shapes.push_back(p_shape);


                } else {
                    cerr << "Warning: Could not read image: "
                         << entryPath.string() << endl;
                }

            }
        }
    }


}

vector<Shape*> SearchEngine::QueryPFEngine(Mat *img)
{

    double t = (double)getTickCount();
    // QUERY PROCESSING

    Mat query_32fc1 = imread("/home/william/Dropbox/GreatCode/SearchWorkBench-build-desktop/sketch.jpg",0);
    //Mat query_32fc1;
    //query.convertTo(query_32fc1,CV_32FC1);
    //int ftype = query_32fc1.type();
    //cv::imwrite("query32.jpg", query);

    Mat img_blur;
    img_blur = query_32fc1.clone();


    blur( query_32fc1, img_blur, cv::Size( 30,30 ), cv::Point(-1,-1) );

    Mat img_blur_gray,img_blur_gray_thres;
    img_blur.convertTo(img_blur_gray,CV_8UC1);
    adaptiveThreshold(img_blur_gray, img_blur_gray_thres,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY_INV,31,2);

    int ftype9 = img_blur_gray_thres.type();
    Mat mul_mat,mul_mat_32fc1;
    cv::gpu::GpuMat gpumat_query,gpu_mat_mul_mat,gpu_mat_mul_mat_32fc1;

    gpumat_query.upload(img_blur_gray_thres);

    std::vector<double> similarity;

    typedef std::pair<int,double> mypair;
    std::vector<mypair> simpairs;
    // Compute responses
    int count = 0;

    for (std::vector<cv::gpu::GpuMat>::iterator it = dbimages.begin() ; it != dbimages.end(); ++it)
    {


        //Mat db_img =  *it;
        cv::gpu::GpuMat gpumat_db_img = *it;
        //gpumat_db_img.upload(db_img);

        //cv::bitwise_and(gpumat_db_img,gpumat_query,gpu_mat_mul_mat);

        cv::gpu::bitwise_and(gpumat_db_img, gpumat_query, gpu_mat_mul_mat);

        // cv::bitwise_and(db_img,img_blur_gray_thres,mul_mat);

        gpu_mat_mul_mat.convertTo(gpu_mat_mul_mat_32fc1,CV_32FC1,1./255);

        //mul_mat.convertTo(mul_mat_32fc1,CV_32FC1,1./255);
        //int ftypemul = mul_mat_32fc1.type();

        Scalar val =  gpu::sum(gpu_mat_mul_mat_32fc1);

        //Scalar val =  sum(mul_mat_32fc1);
        double d_val = val(0);
        similarity.push_back(d_val);
        Shape * p_shape = m_shapes[count];
        p_shape->m_sim = d_val;

        count++;
    }

    vector<Shape*> new_shapes(m_shapes);
    std::sort(new_shapes.begin(), new_shapes.end(), Shape::compareit);
    //std::vector<Shape*>::iterator it2;

    vector<Shape*> ret_vec;
    for (std::vector<Shape*>::iterator it2 = new_shapes.begin() ; it2 != new_shapes.begin()+30; ++it2)
    {
        ret_vec.push_back(*it2);
    }

    // std::copy ( new_shapes.begin(), new_shapes.begin()+20, ret_vec.begin() );

    double t_fin = ((double)getTickCount() - t)/getTickFrequency();
    cout<<"Total time for retrieval t = "<<t_fin<< " secs"<<endl;
    return ret_vec;
}

Mat SearchEngine::GetBICEDescriptor(Mat &img)
{
    double t_in = (double)getTickCount();
    bool bool_time_messages = true;
    bool bool_debug_fun = false;
    Size sz = img.size();
    int patch_width = 100;// assuming a square patch extraction
    //int patch_width_y = 100;
    double overlap = 0.5; // fraction of overlap
    int overlap_width = overlap*patch_width; // overlapping portion of patch with adjacent patch
    int num_bin_y = 8;
    int num_bin_x = 16;
    double tau = 0.2; // threshold for the to top tau*100 percent to be set to 1 in bin descriptor

    int num_theta_classes = 4; // divided 0-180 into classes
    int x_num = (sz.width-patch_width)/(patch_width-overlap_width); // num of patches in x
    int y_num = (sz.height-patch_width)/(patch_width-overlap_width); // num of patches in y dir
    int num_patches = (x_num) * (y_num);
    int descriptor_dim = num_bin_x*num_bin_y*num_theta_classes+1;// length of the descriptor for a single patch

    // allocate memory for descriptor
    Mat descriptor(Size(num_patches,descriptor_dim),CV_8U,Scalar::all(0));



    //Mat img = img2.clone();
    //Mat img = imread("/home/william/Dropbox/GreatCode/SearchWorkBench/lena.jpg",0);
    img.convertTo(img,CV_64FC1);

    int type = img.type();
    if(type!=CV_64FC1)
    {
        cout<<"Image could not be converted to CV_64FC1"<< endl;
        return descriptor;
    }

    Mat desx,grad,desy,avg_grad,grad_norm_mat;
    desx = img.clone();
    desy= img.clone();
    grad = img.clone();
    grad_norm_mat = img.clone();

    int kernel_sz = 5;

    // compute the gradient
    Sobel(img, desx, CV_64FC1, 1, 0, kernel_sz);
    GaussianBlur( desx, desx, Size( kernel_sz, kernel_sz ), 0, 0 );

    Sobel(img, desy, CV_64FC1, 0, 1, kernel_sz);
    GaussianBlur( desy, desy, Size(kernel_sz, kernel_sz ), 0, 0 );

    Sobel(img, grad, CV_64FC1, 1, 1, kernel_sz);
    GaussianBlur( grad, avg_grad, Size( kernel_sz, kernel_sz ), 0, 0 );

    if(bool_debug_fun) cv::imwrite("sobel.jpg", grad);
    if(bool_debug_fun) cv::imwrite("sobel_avg.jpg", avg_grad);

    cv::Mat mat_theta =  Mat::zeros(sz.height,sz.width, CV_64FC1);

    if(bool_debug_fun) cv::imwrite("sobelx.jpg", desx);
    if(bool_debug_fun) cv::imwrite("sobely.jpg", desy);

    for(int row = 0; row < img.rows; ++row) {
        uchar * p = desx.ptr(row);
        for(int col = 0; col < img.cols; ++col) {
            double cx = *p++;

            double x = desx.at<double>(row,col);
            double y = desy.at<double>(row,col);
            double theta = cv::fastAtan2(y,x);

            if(theta>0)
            {
                //cout << theta << endl;
                mat_theta.at<double>(row,col) = theta;
            }

            // normalize grad
            double grad_val = grad.at<double>(row,col);
            if(grad_val<0) grad_val= -1*grad_val;
            double grad_avg = avg_grad.at<double>(row,col);
            if(grad_avg<0)grad_avg = -1*grad_avg;
            double epsil = 4;

            double grad_norm = grad_val/(grad_avg>epsil ? grad_avg:epsil);
            grad_norm_mat.at<double>(row,col) = grad_norm;

        }
    }

    if(bool_debug_fun)cv::imwrite("theta.jpg", mat_theta);
    if(bool_debug_fun)cv::imwrite("gradnorm.jpg", grad_norm_mat);


    // compute patch descriptors
    //Mat patch = cv::Mat::ones(patch_width,patch_width,CV_64FC1);
    //Mat patch_theta = cv::Mat::ones(patch_width,patch_width,CV_64FC1);

    Mat patch;
    Mat patch_theta;


    double t_img_proc = (double)getTickCount();
    int irow,icol,icount=0;
    // Compute normalized gradient and angles for the image
    for ( irow =0; irow <sz.width-patch_width;)// Image to Patches
    {
        for(icol =0;icol <sz.height-patch_width;)// Image to Patches
        {

            patch = grad_norm_mat(cv::Rect(irow,icol,patch_width,patch_width));

            //find patch sum  if its very small dont do anything!!
            Scalar patch_sum = sum(patch);
            if(patch_sum(0)<10)
            {
                double zzz = patch_sum(0);
                icol = icol + overlap_width;
                icount++;
                cout <<"Skip patch. low sum!" << zzz <<endl;
                continue;
            }


            patch_theta = mat_theta(cv::Rect(irow,icol,patch_width,patch_width));

            if(bool_debug_fun)cv::imwrite("patch_theta.jpg", patch_theta);
            if(bool_debug_fun)cv::imwrite("patch.jpg", patch);

            int irow_patch,icol_patch;


            // int sz_patch_accum[] = {num_theta_classes,patch.rows,patch.cols};

            vector<Mat> accum_mat;

            // parallelize this ??
            for(int ichan=0;ichan<num_theta_classes;ichan++)
            {
                Mat th_mat(patch.size(),CV_64FC1,Scalar::all(0)); // initialize each matrix
                accum_mat.push_back(th_mat);
            }


            Mat patch_accum_channel;
            Size szc = patch_accum_channel.size();
            int ch = patch_accum_channel.channels();

            for ( irow_patch =0; irow_patch <patch.cols;irow_patch++)
            {// Patch to Pixel
                for(icol_patch =0;icol_patch <patch.rows;icol_patch++)// Patch to Pixel
                {

                    // rotate the patch about the center with the angle of gradient
                    double angle = patch_theta.at<double>(irow_patch,icol_patch);
                    double grad_norma_val = patch.at<double>(irow_patch,icol_patch);
                    int  theta_class = ClassifyAngle(angle,num_theta_classes);
                    Mat accum_patch = accum_mat[theta_class];
                    // if(bool_debug_fun)cv::imwrite("accum.jpg", accum_patch);
                    Mat result_mat = accum_patch.clone();

                    // Mat result_mat=accum_patch;
                    if(grad_norma_val>0.1) add(accum_patch,patch,result_mat); // accumulate only if an edge exists
                    accum_mat.at(theta_class) = result_mat;
                    //if(bool_debug_fun)cv::imwrite("accum.jpg", result_mat);
                }
            }

            //double t_accum = (double)getTickCount();

            Point center = Point( patch.cols/2, patch.rows/2 );
            double scale = 0.707;//sqrt 2 to preserve most information when rotated
            Mat rot_mat;

            for(int ii_theta=0; ii_theta<num_theta_classes;ii_theta++)
            {
                Mat accum_patch2 = accum_mat[ii_theta];
                double angle2 = ((ii_theta+1)*CV_PI)/4;
                Mat patch_rotated;
                rot_mat = getRotationMatrix2D( center, angle2, scale );
                warpAffine( accum_patch2, patch_rotated, rot_mat, patch.size() );
                accum_mat.at(ii_theta) = patch_rotated;

            }

            double t_patch_in = (double)getTickCount();

            // Compute descriptor for a patch
            Mat patch_bice_mat = ComputeBICEAccumPatchDescriptor(accum_mat,num_theta_classes,num_bin_x,num_bin_y,tau);

            double t_patch_out = (double)getTickCount();
            // if(bool_time_messages)cout<<" t_patch = "<<(t_patch_out - t_patch_in)/getTickFrequency()<< " secs"<<endl;

            // copy the computed descriptor to the matrix descriptor
            cv::Mat row_header_mat = descriptor(cv::Rect(icount,0,1,descriptor_dim));
            patch_bice_mat.copyTo(row_header_mat);




            icol = icol + overlap_width;
            icount++;
        }
        irow = irow + overlap_width;
    }

    double t_desc = (double)getTickCount();
    if(bool_debug_fun)// print the matrix
    {
        FileStorage fs("myFile.yml", FileStorage::WRITE);
        fs<<"descriptor"<<descriptor;
        fs.release();
    }

    double t_fin = (double)getTickCount();
    if(bool_time_messages)cout<<"Total time for computation of descriptor = "<<(t_fin - t_in)/getTickFrequency()<< " secs"<<endl;
    if(bool_time_messages)cout<<"Time for gradient computation = "<<(t_img_proc - t_in)/getTickFrequency()<< " secs"<<endl;
    //if(bool_time_messages)cout<<"Total time for theta binning = "<<(t_accum - t_img_proc)/getTickFrequency()<< " secs"<<endl;
    if(bool_time_messages)cout<<"Time for patchwise descriptor generation = "<<(t_fin - t_img_proc)/getTickFrequency()<< " secs"<<endl;
    return descriptor;
}

Mat SearchEngine::ComputeBICEAccumPatchDescriptor2(vector<Mat> &accum_mat,int &num_theta_classes,int &num_bin_x,int & num_bin_y,double &tau)
{
    bool bool_debug_fun = false;
    Mat patch = accum_mat[0];
    if(bool_debug_fun)cv::imwrite("accum_patch.jpg", patch);
    int bin_width_x = patch.cols/num_bin_x;
    int bin_width_y = patch.rows/num_bin_y;

    int top_tau = tau* num_bin_x*num_bin_y;
    Mat patch_desc(Size(1,num_bin_x*num_bin_y*num_theta_classes+1),CV_8U,Scalar::all(0)); // patch descriptor

    // bin the theta patches
    for(int i_theta=0;i_theta<num_theta_classes;i_theta++)
    {
        Mat mat_res,mat_res2,mat_res3;
        //Mat desc = patch_desc(cv::Rect(0,i_theta*num_bin_x*num_bin_y+1, 1,num_bin_x*num_bin_y ));

        Mat theta_mat_ch = accum_mat.at(i_theta);
        theta_mat_ch.convertTo(theta_mat_ch,CV_8U);

        if(bool_debug_fun)// print the matrix
        {
            FileStorage fs("myFile.yml", FileStorage::WRITE);
            fs<<"descriptor"<<theta_mat_ch;
            fs.release();
        }


        //cv::adaptiveThreshold(mat_res2, mat_res, 1, ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY, 3, 0);


        cv::resize(theta_mat_ch,mat_res3,Size(num_bin_x,num_bin_y));
        threshold(mat_res3, mat_res3, 0.1,1, THRESH_BINARY);
        //mat_res.convertTo(theta_mat_ch,CV_32F);
        //
        //cv::adaptiveThreshold(mat_res, mat_res2, 1, ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY, 3, 0);

        // desc(mat_res3.reshape(1,num_bin_y*num_bin_x))
        patch_desc(cv::Rect(0,i_theta*num_bin_x*num_bin_y+1, 1,num_bin_x*num_bin_y ))  = mat_res3.reshape(1,num_bin_y*num_bin_x);
        //Mat temp = mat_res3.reshape(1,num_bin_y*num_bin_x);
        //mat_res3.reshape(1,num_bin_y*num_bin_x).copyTo(patch_desc(cv::Rect(0,i_theta*num_bin_x*num_bin_y+1, 1,num_bin_x*num_bin_y )));
        //temp.copyTo(desc);

    }


    return patch_desc;

}



Mat SearchEngine::ComputeBICEAccumPatchDescriptor(vector<Mat> &accum_mat,int &num_theta_classes,int &num_bin_x,int & num_bin_y,double &tau)
{
    bool bool_debug_fun = false;
    Mat patch = accum_mat[0];
    if(bool_debug_fun)cv::imwrite("accum_patch.jpg", patch);
    int bin_width_x = patch.cols/num_bin_x;
    int bin_width_y = patch.rows/num_bin_y;

    int top_tau = tau* num_bin_x*num_bin_y;

    // add one in the size otherwise the system crashes ??
    Mat patch_desc(Size(1,num_bin_x*num_bin_y*num_theta_classes+1),CV_8U,Scalar::all(0)); // patch descriptor

    // bin the theta patches
    for(int i_theta=0;i_theta<num_theta_classes;i_theta++)
    {
        Mat desc_mat_ch(Size(num_bin_x,num_bin_y),CV_64FC1,Scalar::all(0));
        Mat desc = patch_desc(cv::Rect(0,i_theta*num_bin_x*num_bin_y+1, 1,num_bin_x*num_bin_y ));

        vector<double> desc_vector;
        Mat theta_mat_ch = accum_mat.at(i_theta);

        for(int i_x_patch=0;i_x_patch<patch.cols-2*bin_width_x;)
        {
            for(int i_y_patch=0; i_y_patch<patch.rows-2*bin_width_y; )
            {
                int x_bin = i_x_patch/bin_width_x;
                int y_bin = i_y_patch/bin_width_y;

                Mat sub_patch =  theta_mat_ch(cv::Rect(i_x_patch,i_y_patch,bin_width_x,bin_width_y));
                Scalar bin_val = cv::sum(sub_patch);
                double val_bin_desc = bin_val[0];
                desc_vector.push_back(val_bin_desc);
                desc_mat_ch.at<double>(y_bin,x_bin)=bin_val[0];

                i_y_patch = i_y_patch + bin_width_y;
            }

            i_x_patch = i_x_patch +bin_width_x ;
        }

        int tempsize =desc_vector.size();
        std::sort(desc_vector.begin(),desc_vector.end(), std::greater<int>());
        double thres_desc = desc_vector[top_tau];
        double sum_of_tau_elems =std::accumulate(desc_vector.begin(),desc_vector.begin()+top_tau,0);
        desc_vector.empty(); //empty the vector so that it does not accumulate over the older vals


        if(sum_of_tau_elems <=0) // find the case where every thing is zero
        {
            cout<<"The threshold is Zero. Returning zero patch decriptor descriptor for theta"<<i_theta <<endl;
        }
        else
        { // assign descriptors

            int i_desc=0;
            Mat temp_desc = desc_mat_ch.clone();
            temp_desc= temp_desc.reshape(1,num_bin_y*num_bin_x);
            for( i_desc=0;i_desc<num_bin_y*num_bin_x;i_desc++)
            {
                double dv = temp_desc.at<double>(i_desc,0);
                if(dv>thres_desc)
                {
                    desc.at<int>(i_desc,0) = 1; // set the element value to 1
                }
            }
            //desc_vector.empty();


            Scalar desc_sum = sum(desc);
            if(desc_sum[0]<top_tau)
            {
                cout<<"Number of Descriptor bins are less !"<<desc_sum[0]<<endl;
            }
        }

        // Copy descriptor row over to the larger matrix

    }



    return patch_desc;

}

void SearchEngine::DrawGrid(int &numx_cell,int &numy_cell, Mat &img)
{
    int distx=img.cols/numx_cell;
    int disty =img.rows/numy_cell;

    int width=img.size().width;
    int height=img.size().height;

    for(int i=0;i<height;i+=disty)
        cv::line(img,Point(0,i),Point(width,i),cv::Scalar(255,255,255));

    for(int i=0;i<width;i+=distx)
        cv::line(img,Point(i,0),Point(i,height),cv::Scalar(255,255,255));

    //    for(int i=0;i<width;i+=dist)
    //      for(int j=0;j<height;j+=dist)
    //        mat.at<cv::Vec3b>(i,j)=cv::Scalar(10,10,10);
}

// Classifies bins the angle into one of the  classes it belongs to
// num_classes: Number of classes to divide the 0-180 into
// angle input angle
// returns the class number;
int  SearchEngine::ClassifyAngle(double &angle, int &num_classes)
{
    int class_id = 0;

    int angle_int = (int(angle+0.5)) % 180;// convert all angles to less than 180 ints
    int min_value = 0; // lower limit
    int max_value = 180; // upper limit
    int bin_width = (max_value-min_value)/num_classes;

    class_id = (angle_int-min_value)/bin_width;
    return class_id;
}

int SearchEngine::Bin(double &val,double &ll,double &ul,int &num_classes)
{
    int class_id = 0;
    double bin_width = (ul-ll)/num_classes;
    class_id = int((val-ll)/bin_width);
    return class_id;
}

Mat SearchEngine::GetBICEDescriptorParallel(Mat &img)
{

    cout<<"Running BICE Descriptor Parallel"<<endl;
    // QCoreApplication a(argc, argv);

    bool bool_debug_fun = false;
    bool silent = false;

    Mat desx,grad,desy,avg_grad,grad_norm_mat;

    // Mat img = imread("/home/william/Dropbox/GreatCode/Indexer/lena.jpg",0);
    img.convertTo(img,CV_64FC1);

    int type = img.type();
    if(type!=CV_64FC1)
    {
        std::cout<<"Image could not be converted to CV_64FC1"<< endl;
        //return 0;
    }
    desx = img.clone(); // actual memory copy
    desy= img.clone(); // actual memory copy
    grad = img.clone();

    size_t m = img.rows;
    size_t n = img.cols;
    cv::Size sz= Size(m,n);


    int kernel_sz = 3;
    // compute the gradient
    Sobel(img, desx, CV_64FC1, 1, 0, kernel_sz);
    GaussianBlur( desx, desx, Size( kernel_sz, kernel_sz ), 0, 0 );

    Sobel(img, desy, CV_64FC1, 0, 1, kernel_sz);
    GaussianBlur( desy, desy, Size(kernel_sz, kernel_sz ), 0, 0 );

    Sobel(img, grad, CV_64FC1, 1, 1, kernel_sz);
    GaussianBlur( grad, avg_grad, Size( kernel_sz, kernel_sz ), 0, 0 );

    if(bool_debug_fun) cv::imwrite("sobelx.jpg", desx);
    if(bool_debug_fun) cv::imwrite("sobely.jpg", desy);
    if(bool_debug_fun) cv::imwrite("sobel.jpg", grad);


    cv::Mat * g_x = new cv::Mat(desx);
    cv::Mat * g_y = new cv::Mat(desy);
    cv::Mat *g = new cv::Mat(grad);
    cv::Mat *g_avg = new cv::Mat(avg_grad);
    //cv::Mat * g_x = &desx;
    // cv::Mat * g_y = &desy;


    cv::Mat theta = cv::Mat::zeros(sz,CV_64FC1);
    cv::Mat * out_t = &theta;

    //cv::Mat * out_t = new cv::Mat(desx);
    //cv::Mat * out_g = new cv::Mat(sz,CV_64FC1);

    int num_theta_classes = 4; // change later to a param
    //cv::Mat** OutArray = new cv::Mat*[num_classes];

    std::vector<cv::Mat> listOfMatrices;

    // allocate memory for each mat
    for(int im =0;im<num_theta_classes;im++)
    {
        Mat m = cv::Mat::zeros(sz,CV_64FC1);
        listOfMatrices.push_back(m);
    }

    tbb::task_scheduler_init init;
    tbb::tick_count t0 = tbb::tick_count::now();

    tbb::parallel_for(tbb::blocked_range2d<size_t>(0,m,2,0,n,2),
                      ApplyClassifyTheta(g,g_x,g_y,g_avg,out_t,listOfMatrices),tbb::auto_partitioner());

    tbb::tick_count t1 = tbb::tick_count::now();

    /* split the data into patches */
    int num_bin_y = 8;
    int num_bin_x = 16;
    double tau = 0.1; // threshold for the to top tau*100 percent to be set to 1 in bin descriptor

    std::vector<cv::Mat> listOfPatches;
    std::vector<int> theta_class_id;

    int descriptor_dim = num_bin_x*num_bin_y+1;// length of the descriptor for a single patch
    int patch_width = 100;// assuming a square patch extraction
    //int patch_width_y = 100;
    double overlap = 0.5; // fraction of overlap
    int overlap_width = overlap*patch_width; // overlapping portion of patch with adjacent patch

    int x_num = (sz.width-patch_width)/(patch_width-overlap_width); // num of patches in x
    int y_num = (sz.height-patch_width)/(patch_width-overlap_width); // num of patches in y dir
    int num_patches = (x_num) * (y_num) *num_theta_classes ;

    // generate patches

    int patch_count = 0;


    for (int  irow =0; irow <sz.width-patch_width;)// for each row of patches
    {
        for(int icol =0;icol <sz.height-patch_width;)// for each col of patches
        {

            for (int itheta = 0;itheta<num_theta_classes;itheta++){ // for each theta
                Mat temp = listOfMatrices[itheta](cv::Rect(irow,icol,patch_width,patch_width)); // get header to the region
                Mat patch = temp.clone(); // deep copy data each patch has its own memory allocated

                listOfPatches.push_back(patch);// push back
                theta_class_id.push_back(itheta);
                patch_count++;
            }
            icol = icol + overlap_width;
        }
        irow = irow + overlap_width;
    }


    //Mat descriptor(Size(descriptor_dim,patch_count),CV_8U,Scalar::all(0)); // patches arranged vertically
    //cv::Mat descriptor = cv::Mat::zeros(Size(descriptor_dim,patch_count),CV_8UC1);
    cv::Mat descriptor = cv::Mat::zeros(Size(descriptor_dim,patch_count),CV_8UC1);
    cv::Mat * ptr_desc = &descriptor;



    tbb::tick_count t2 = tbb::tick_count::now();

    tbb::parallel_for(tbb::blocked_range<size_t>(0,patch_count,1),
                      ComputeBICEPatchDescriptors(listOfPatches,theta_class_id,ptr_desc,num_theta_classes,num_bin_x, num_bin_y,tau),tbb::auto_partitioner());

    tbb::tick_count t3 = tbb::tick_count::now();
    init.terminate();

    cv::Mat result = descriptor.reshape ( 0, 1 );
    if(bool_debug_fun)// print the matrix
    {
        FileStorage fs("myFile.yml", FileStorage::WRITE);
        fs<<"descriptor"<<result;
        fs.release();
    }
    // QString  m_data_dir = QString("/home/william/Datasets/allpics");
    // SearchEngine * engine = new SearchEngine("HARRIS","ORB","BruteForce-Hamming");
    //engine->StartBICEngine(m_data_dir.toStdString());
    if ( !silent ) printf("time for gradient normalization= %g\n", (t1-t0).seconds());
    if ( !silent ) printf("time for computing patch descriptors  = %g\n", (t3-t2).seconds());

    return result;
}




