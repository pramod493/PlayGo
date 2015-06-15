#include "graphicsitemgroup.h"
namespace CDI
{
	GraphicsItemGroup::GraphicsItemGroup(QGraphicsItem *graphicsParent)
		: QGraphicsItemGroup(graphicsParent)
    {
        // Make sure that each object added to this is also of type CDI::Item
//        setAcceptDrops(true);
//        setAcceptTouchEvents(true);
    }

    GraphicsItemGroup::~GraphicsItemGroup()
    {
    }

	void GraphicsItemGroup::updateGroup()
	{
		if (component!= NULL)
		{
			setTransform(component->transform());
			update();
		}
	}
}
