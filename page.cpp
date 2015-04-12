#include "page.h"
namespace CDI
{

    Page::Page(QObject *parent) : QObject(parent),
        isCurrent(false)
    {

    }

    Page::~Page()
    {

    }

    void Load()
    {

    }

    void Close()
    {

    }

    Page* Clone()
    {
        // Deep copy
        return NULL;
    }

}
