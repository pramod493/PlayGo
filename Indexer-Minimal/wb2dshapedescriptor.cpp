#include "wb2dshapedescriptor.h"

wb2DShapeDescriptor::wb2DShapeDescriptor()
{

    m_verbose = true; // print out the actions on the command prompt
    m_debug = true; // save the intermediate images
}

wb2DShapeDescriptor::~wb2DShapeDescriptor()
{
}


std::string wb2DShapeDescriptor::GetName()
{
    return m_name;

}
