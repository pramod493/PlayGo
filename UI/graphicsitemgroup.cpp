#include "graphicsitemgroup.h"
namespace CDI
{
    GraphicsItemGroup::GraphicsItemGroup(QGraphicsItem *graphicsParent, QObject* objectParent)
		: QGraphicsItemGroup(graphicsParent)
    {
        // Make sure that each object added to this is also of type CDI::Item
        setAcceptDrops(true);
        setAcceptTouchEvents(true);
    }

    GraphicsItemGroup::~GraphicsItemGroup()
    {

    }
}
