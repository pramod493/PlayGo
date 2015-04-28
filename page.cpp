#include "page.h"
namespace CDI
{

    Page::Page(QObject *parent) : QObject(parent),
        _isCurrent(false)
    {
        _parent = parent;
    }

    Page::~Page()
    {

    }

    void Page::Load()
    {

    }

    void Page::Close()
    {

    }

    Page* Page::Clone()
    {
        Page* clonePage = new Page(_parent);
        return clonePage;
    }

}
