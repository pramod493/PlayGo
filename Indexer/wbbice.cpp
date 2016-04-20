#include "tbb/tbb.h"
#include "tbb/parallel_for.h"
#include "wbbice.h"
#include "parallelbice.h"

using namespace std;
using namespace boost::filesystem;
using namespace cv;

wbBICE::wbBICE()
	: m_num_bin_y(8),m_num_bin_x(16),m_kernel_sz(3),m_num_theta_classes(4),
	  m_tau(0.1),m_patch_width(100),m_overlap(0.5)
{
	m_name = "bice";
}

void wbBICE::ReadSettings(path settingspath)
{
	m_settingspath = settingspath;
}
void wbBICE::WriteSettings(path settingspath)
{
	m_settingspath = settingspath;
}

cv::Mat wbBICE::GetDescriptor(cv::Mat &img)
{
	cout<<"Running BICE Descriptor Parallel"<<endl;

	Mat desx,grad,desy,avg_grad,grad_norm_mat;
	resize(img,img, Size(300,300));
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

	// compute the gradient
	Sobel(img, desx, CV_64FC1, 1, 0, m_kernel_sz);
	GaussianBlur( desx, desx, Size( m_kernel_sz, m_kernel_sz ), 0, 0 );

	Sobel(img, desy, CV_64FC1, 0, 1, m_kernel_sz);
	GaussianBlur( desy, desy, Size(m_kernel_sz, m_kernel_sz ), 0, 0 );

	Sobel(img, grad, CV_64FC1, 1, 1, m_kernel_sz);
	GaussianBlur( grad, avg_grad, Size( m_kernel_sz, m_kernel_sz ), 0, 0 );

	if(m_debug)
	{
		cv::imwrite("sobelx.jpg", desx);
		cv::imwrite("sobely.jpg", desy);
		cv::imwrite("sobel.jpg", grad);
	}


	cv::Mat * g_x = new cv::Mat(desx);
	cv::Mat * g_y = new cv::Mat(desy);
	cv::Mat *g = new cv::Mat(grad);
	cv::Mat *g_avg = new cv::Mat(avg_grad);

	cv::Mat theta = cv::Mat::zeros(sz,CV_64FC1);
	cv::Mat * out_t = &theta;

	std::vector<cv::Mat> listOfMatrices;
	// allocate memory for each mat
	for(int im =0;im<m_num_theta_classes;im++)
	{
		Mat m = cv::Mat::zeros(sz,CV_64FC1);
		listOfMatrices.push_back(m);
	}

	tbb::task_scheduler_init init;
	tbb::tick_count t0 = tbb::tick_count::now();

	tbb::parallel_for(tbb::blocked_range2d<size_t>(0,m,2,0,n,2),
					  ApplyClassifyTheta(g,g_x,g_y,g_avg,out_t,listOfMatrices),tbb::auto_partitioner());

	tbb::tick_count t1 = tbb::tick_count::now();
	std::vector<cv::Mat> listOfPatches;
	std::vector<int> theta_class_id;

	int descriptor_dim = m_num_bin_x*m_num_bin_y+1;// length of the descriptor for a single patch
	int overlap_width = m_overlap*m_patch_width; // overlapping portion of patch with adjacent patch

	int patch_count = 0;

	for (int  irow =0; irow <sz.width-m_patch_width;)// for each row of patches
	{
		for(int icol =0;icol <sz.height-m_patch_width;)// for each col of patches
		{
			for (int itheta = 0;itheta<m_num_theta_classes;itheta++){ // for each theta
				Mat temp = listOfMatrices[itheta](cv::Rect(irow,icol,m_patch_width,m_patch_width)); // get header to the region
				Mat patch = temp.clone(); // deep copy data each patch has its own memory allocated

				listOfPatches.push_back(patch);// push back
				theta_class_id.push_back(itheta);
				patch_count++;
			}
			icol = icol + overlap_width;
		}
		irow = irow + overlap_width;
	}
	cv::Mat descriptor = cv::Mat::zeros(Size(descriptor_dim,patch_count),CV_8UC1);
	cv::Mat * ptr_desc = &descriptor;

	tbb::tick_count t2 = tbb::tick_count::now();

	tbb::parallel_for(tbb::blocked_range<size_t>(0,patch_count,1),
					  ComputeBICEPatchDescriptors(listOfPatches,theta_class_id,ptr_desc,m_num_theta_classes,m_num_bin_x, m_num_bin_y,m_tau),tbb::auto_partitioner());

	tbb::tick_count t3 = tbb::tick_count::now();
	init.terminate();

	cv::Mat result = descriptor.reshape ( 0, 1 );
	if(m_debug)// print the matrix
	{
		FileStorage fs("myFile.yml", FileStorage::WRITE);
		fs<<"descriptor"<<result;
		fs.release();
	}
	if ( m_verbose ) cout<<"time for gradient normalization="<<(t1-t0).seconds()<<endl;
	if (m_verbose ) cout<<"time for computing patch descriptors  = %g\n"<< (t3-t2).seconds()<<endl;

	img.release();
	desx.release();
	grad.release();
	desy.release();
	avg_grad.release();
	grad_norm_mat.release();

	return result;
}
