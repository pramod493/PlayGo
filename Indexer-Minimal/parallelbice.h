#ifndef PARALLELBICE_H
#define PARALLELBICE_H

#include "tbb/blocked_range2d.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <numeric>
using namespace tbb;

class ApplyClassifyTheta{
    cv::Mat * in_grad_x;
    cv::Mat * in_grad_y;
    cv::Mat * in_grad_avg;
    cv::Mat * in_grad;
    cv::Mat * out_theta;
    //cv::Mat ** gradarray;
    std::vector<cv::Mat> listOfMatrices;


public:
    void operator()( const blocked_range2d<size_t>& r ) const {
        for( size_t i=r.rows().begin(); i!=r.rows().end(); ++i ){
            for( size_t j=r.cols().begin(); j!=r.cols().end(); ++j ) {

                double x = in_grad_x->at<double>(i,j);
                double y = in_grad_y->at<double>(i,j);
                double angle = cv::fastAtan2(y,x);

                out_theta->at<double>(i,j) = angle;

                int class_id = 0;
                int angle_int = (int(angle+0.5)) % 180;// convert all angles to less than 180 ints
                int min_value = 0; // lower limit
                int max_value = 180; // upper limit
                int num_classes = 4; // change later to a param
                int bin_width = (max_value-min_value)/num_classes;

                class_id = (angle_int-min_value)/bin_width;
                //gradarray[class_id]->at<double>(i,j) =
                cv::Mat gthetamat = listOfMatrices[class_id];

                double grad_avg = in_grad_avg->at<double>(i,j);
                if(grad_avg<0)grad_avg = -1*grad_avg;
                double epsil = 4;

                double grad_val = in_grad->at<double>(i,j);
                double grad_norm = grad_val/(grad_avg>epsil ? grad_avg:epsil);
                gthetamat.at<double>(i,j) = grad_norm;
            }
        }
    }

    // Constructor
    ApplyClassifyTheta(cv::Mat *g,cv::Mat *g_x, cv::Mat * g_y,cv::Mat * g_avg,cv::Mat * out_t,std::vector<cv::Mat> matvec)
        : in_grad_x(g_x), in_grad_y(g_y),in_grad_avg(g_avg),in_grad(g), out_theta(out_t),listOfMatrices(matvec)
    {

    }
};

class ComputeBICEPatchDescriptors{
    std::vector<cv::Mat> listOfPatches;
    std::vector<int> theta_class;
    //std::vector<int> error_id;
    cv::Mat * m_desc;
    int m_num_theta;
    int m_num_bin_x;
    int m_num_bin_y;
    double m_tau;

public:
    void operator()(const tbb::blocked_range<size_t>& r) const {
        for (size_t i=r.begin();i!=r.end();++i){

            cv::Mat patch= listOfPatches[i];
            double cid = theta_class[i];

            // rotate path
            double angle2 = ((cid)*CV_PI)/m_num_theta;
            cv::Point center =cv::Point( patch.cols/2, patch.rows/2 );
            double scale = 0.707;//sqrt 2 to preserve most information when rotated
            cv::Mat rot_mat = getRotationMatrix2D( center, angle2, scale );
            warpAffine( patch, patch, rot_mat, patch.size() );

            // binarize
            int bin_width_x = patch.cols/m_num_bin_x;
            int bin_width_y = patch.rows/m_num_bin_y;
            int top_tau = m_tau* m_num_bin_x*m_num_bin_y;

            cv::Mat desc_mat_ch(cv::Size(m_num_bin_x,m_num_bin_y),CV_64FC1,cv::Scalar::all(0));
            std::vector<double> desc_vector;
            //Mat theta_mat_ch = accum_mat.at(i_theta);

            for(int i_x_patch=0;i_x_patch<patch.cols-2*bin_width_x;)
            {
                for(int i_y_patch=0; i_y_patch<patch.rows-2*bin_width_y; )
                {
                    int x_bin = i_x_patch/bin_width_x;
                    int y_bin = i_y_patch/bin_width_y;

                    cv::Mat sub_patch =  patch(cv::Rect(i_x_patch,i_y_patch,bin_width_x,bin_width_y));
                    cv::Scalar bin_val = cv::sum(sub_patch);
                    double val_bin_desc = bin_val[0];
                    desc_vector.push_back(val_bin_desc);
                    desc_mat_ch.at<double>(y_bin,x_bin)=bin_val[0];

                    i_y_patch = i_y_patch + bin_width_y;
                }

                i_x_patch = i_x_patch +bin_width_x ;
            }

            std::sort(desc_vector.begin(),desc_vector.end(), std::greater<int>());
            double thres_desc = desc_vector[top_tau];

            int i_desc=0;

            cv::Mat desc = m_desc->row(i);
            cv::Mat temp_desc = desc_mat_ch.clone();
            temp_desc= temp_desc.reshape(1,m_num_bin_y*m_num_bin_x);
            for( i_desc=0;i_desc<desc.cols;i_desc++)
            {
                double dv = temp_desc.at<double>(i_desc,0);
                if(dv>thres_desc)
                {
                    //  desc.at<int>(0,i_desc) =3; // set the element value to 1
                    m_desc->at<bool>(i,i_desc) = 1;
                }
            }

        }
    }

    ComputeBICEPatchDescriptors( std::vector<cv::Mat> patches,std::vector<int> classids,cv::Mat* desc,int num_theta,int num_bin_x, int num_bin_y,double tau)
        :listOfPatches(patches),theta_class(classids),m_desc(desc),m_num_theta(num_theta),m_num_bin_x(num_bin_x),m_num_bin_y(num_bin_y),m_tau(tau)
    {
    }
};
#endif // PARALLELBICE_H
