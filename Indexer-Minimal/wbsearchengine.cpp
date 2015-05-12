#include <boost/filesystem.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "wb2dshapedescriptor.h"
#include "wbsearchengine.h"

using namespace std;
using namespace boost::filesystem;
using namespace cv;

wbSearchEngine::wbSearchEngine(path & top_level_folder,wb2DShapeDescriptor * descriptor)
    :m_db_folder(top_level_folder),m_descriptor(descriptor)
{
    m_is_indexed = false;


    if (is_directory(m_db_folder)) {
        cout  << m_db_folder.string()<< "exists" << endl;
    }
    else
    {
        cout  << m_db_folder.string()<< "does not exist" << endl;
    }
}

void wbSearchEngine::Index()
{
    boost::filesystem::path image_dir = m_db_folder / "images";
    if(!exists( image_dir ))
    {
        cout<< "No Image data found. Error!"<< endl;
        return;
    }

    // save the descriptor for the file.
    //boost::filesystem::path dir = m_db_folder.parent_path();
    std::string desc_name = m_descriptor->GetName();
    boost::filesystem::path desc_dir = m_db_folder / desc_name;

    if(exists( desc_dir ))

    {
        int image_count = 0;
        int index_files_count = 0;

        for (directory_iterator iter = directory_iterator(image_dir); iter
             != directory_iterator(); iter++) {
            directory_entry entry = *iter;
            path entryPath = entry.path();
            if ((entryPath.extension() == ".png") | (entryPath.extension() == ".jpg")) image_count++;
        }


        for (directory_iterator iter2 = directory_iterator(desc_dir); iter2
             != directory_iterator(); iter2++) {
            directory_entry entry = *iter2;
            path entryPath = entry.path();
            if (entryPath.extension() == ".yml" )index_files_count++;
        }
        if(image_count==index_files_count) // verfying if the number of index files is the same as image files
        {
            cout<< "The database is indexed"<< endl;
            m_is_indexed = true;
            return;

        }


    }
    else
    {
        create_directory(desc_dir);
    }


    for (directory_iterator iter = directory_iterator(image_dir); iter
         != directory_iterator(); iter++) {
        directory_entry entry = *iter;

        path entryPath = entry.path();
        if ((entryPath.extension() == ".png") | (entryPath.extension() == ".jpg")) {

            cout << "Indexing file " << entryPath.string()<< "for"<<desc_name<<"descriptor" << endl;
            Mat img = imread(entryPath.string(),0);
            if (!img.empty()) {


                Mat descriptor = m_descriptor->GetDescriptor(img);


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

    m_is_indexed = true;

}

void wbSearchEngine::Load()
{
    std::string desc_name = m_descriptor->GetName();
    if(!m_is_indexed)
    {
        cout<< "Database is not indexed for "<< desc_name<<endl;
        return;
    }


    boost::filesystem::path desc_dir = m_db_folder / desc_name;

    for (directory_iterator iter = directory_iterator(desc_dir); iter
         != directory_iterator(); iter++) {
        directory_entry entry = *iter;

        path entryPath = entry.path();
        if (entryPath.extension() == ".yml" ) {

            cout << "loading  file " << entryPath.string()<< "for "<<desc_name <<" descriptor" << endl;
            cv::FileStorage fs(entryPath.string(), FileStorage::READ );
            Mat descriptor;
            std::string str;
            fs["filepath"]>>str;
            fs["descriptor"]>>descriptor;

            m_descriptors.push_back(descriptor); // save images
            //files.push_back(str); // save the filenames
            wbShape * p_shape = new wbShape(str);
            m_shapes.push_back(p_shape);

            fs.release();

        }
    }


}

vector<wbShape*>  wbSearchEngine::Query(Mat &img, int k)
{
    double tin = (double)getTickCount();

    Mat query_desc = m_descriptor->GetDescriptor(img);

    // Unused
    //Scalar sum_query = cv::sum(query_desc);

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
        wbShape * p_shape = m_shapes[count];
        p_shape->m_sim = d_val;
        count++;
    }

    vector<wbShape*> new_shapes(m_shapes);
    std::sort(new_shapes.begin(), new_shapes.end(), wbShape::compareit);
    //std::vector<Shape*>::iterator it2;

    vector<wbShape*> ret_vec;
    for (std::vector<wbShape*>::iterator it2 = new_shapes.begin() ; it2 != new_shapes.begin()+k; ++it2)
    {
        ret_vec.push_back(*it2);
    }
    double t_fin = (double)getTickCount();

    // Time related output
    cout<<"Total time for retrieval t = "<<(t_fin - tin)/getTickFrequency()<< " secs"<<endl;
    cout<<"Time for descriptor computation t_comp = "<<(t_comp - tin)/getTickFrequency()<< " secs"<<endl;
    cout<<"Time for querying t_comp = "<<(t_fin - t_comp)/getTickFrequency()<< " secs"<<endl;

    return ret_vec;
}

vector<std::string> wbSearchEngine::Query(std::string query_str,int k)
{

    Mat img = imread(query_str,0);      // Read image

    resize(img,img, Size(300,300));     // Resize to 300x300px

    vector<wbShape*> results = Query(img, k);   // Query the results

    boost::filesystem::path results_folder = m_db_folder / "results" ;
    if(!exists(results_folder)){
        create_directory(results_folder);
    }

    boost::filesystem::path results_file = results_folder / "results.yml" ;

    FileStorage fs(results_file.string(), FileStorage::WRITE);

    vector<std::string> ret_vec;
    for (std::vector<wbShape*>::iterator it2 = results.begin() ; it2 != results.begin()+k; ++it2)
    {
        wbShape * shape = *it2;
        fs << "filepath"<<shape->m_file_path;
        ret_vec.push_back(shape->m_file_path);
    }

    fs.release();
    return ret_vec;
}


