#include "wb2dshapedescriptor.h"

wb2DShapeDescriptor::wb2DShapeDescriptor()
{
    m_verbose = false; // print out the actions on the command prompt
    m_debug = false; // save the intermediate images
}

wb2DShapeDescriptor::~wb2DShapeDescriptor()
{
}

std::string wb2DShapeDescriptor::GetName()
{
    return m_name;

}
