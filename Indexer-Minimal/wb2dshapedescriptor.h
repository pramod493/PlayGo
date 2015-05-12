#ifndef WB2DSHAPEDESCRIPTOR_H
#define WB2DSHAPEDESCRIPTOR_H

#include <opencv2/opencv.hpp>
#include <boost/filesystem.hpp>

using namespace cv;
using namespace boost::filesystem;

class wb2DShapeDescriptor
{
public:
    wb2DShapeDescriptor();
    ~wb2DShapeDescriptor();
    //virtual void Index() = 0;
    virtual cv::Mat GetDescriptor(cv::Mat &img) = 0;
    virtual void ReadSettings(path settingspath) = 0;
    virtual void WriteSettings(path settingspath) = 0;
    std::string GetName();

protected:
    bool m_verbose ; // print out the actions on the command prompt
    bool m_debug ; // save the intermediate images
    std::string m_name;
};


#endif // WB2DSHAPEDESCRIPTOR_H
