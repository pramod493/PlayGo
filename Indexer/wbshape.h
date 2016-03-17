#ifndef WBSHAPE_H
#define WBSHAPE_H

#include <string>
class wbShape
{
public:
    wbShape(std::string &str_name)
        :m_sim(0), m_file_path(str_name)
    {

    }
    static bool compareit(wbShape *a, wbShape *b)
    {
        return a->m_sim > b->m_sim;
    }

public:
    double m_sim;
    std::string m_file_path;
};

#endif // WBSHAPE_H
