#include "cdigraphicspathitem.h"
namespace CDI
{
    CDIGraphicsPathItem::CDIGraphicsPathItem(QGraphicsItem *parent, Stroke* stroke)
        : QGraphicsPathItem(parent)
    {
        if (stroke == NULL)
        {
            parentStroke = new Stroke();
        }
        else
        {
            parentStroke = stroke;
        }
        isOrphan = true;
    }

    CDIGraphicsPathItem::~CDIGraphicsPathItem()
    {
        if (parentStroke!= NULL) delete parentStroke;
    }

    void CDIGraphicsPathItem::push_back(QPointF* points)
    {

    }


}
