#ifndef WBBICE_H
#define WBBICE_H
#include"wb2dshapedescriptor.h"

class wbBICE : public wb2DShapeDescriptor
{
public:
    wbBICE();
    ~wbBICE();
    cv::Mat GetDescriptor(cv::Mat &img);
    void  ReadSettings(path settingspath);
    void WriteSettings(path settingspath);
    //std::string GetName();

private:
    int m_num_bin_y;
    int m_num_bin_x;
    int m_kernel_sz;
    int m_num_theta_classes;
    double m_tau;
    int m_patch_width;// assuming a square patch extraction
    //int patch_width_y = 100;
    double m_overlap;
    path m_settingspath;


};

#endif // WBBICE_H
