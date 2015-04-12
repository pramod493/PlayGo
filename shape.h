#pragma once

#include "cdicommon.h"
#include "item.h"

namespace CDI
{
    class Shape : public Item
    {
    public:

        virtual Shape* Clone() (return NULL;);

    };
}
