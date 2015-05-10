#ifndef WBSEARCHENGINE_H
#define WBSEARCHENGINE_H

#include <boost/filesystem.hpp>
#include <opencv2/opencv.hpp>
#include "wb2dshapedescriptor.h"
#include "wbshape.h"
#include <vector>

using namespace boost::filesystem;
using namespace std;
class wbSearchEngine
{
public:
    wbSearchEngine(path & top_level_folder,wb2DShapeDescriptor * descriptor);
    virtual void Index();
    virtual void Load();
    virtual vector<wbShape*>  Query(Mat &img,int k =20);
    void Query(std::string query_str,int k =20);
protected:
    path m_db_folder;
    bool m_is_indexed;
    wb2DShapeDescriptor * m_descriptor;
    vector<wbShape*> m_shapes;
    vector<cv::Mat> m_descriptors;

};

#endif // WBSEARCHENGINE_H
