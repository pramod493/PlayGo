#include <QTouchEvent>
#include <QEvent>
#include <QGraphicsSceneMouseEvent>
#include "sketchscene.h"
#include "QsLog.h"
#include "page.h"

namespace CDI
{
	SketchScene::SketchScene()
		:QGraphicsScene()
	{
		_page = NULL;
	}

	SketchScene::SketchScene(Page *page) :
		QGraphicsScene(page)
	{
		_page = page;
	}

	SketchScene::~SketchScene()
	{
		// Since sketch scene does not create these items,
		// we should simply remove them from scene
		clear();
	}

	void SketchScene::clear()
	{
		QList<QGraphicsItem*> allitems = items();
		for (int i=0; i < allitems.size(); i++)
		{
			QGraphicsItem* graphicsitem = allitems[i];
			removeItem(graphicsitem);
		}
	}

	bool SketchScene::event(QEvent *sceneEvent)
	{
		bool retval = QGraphicsScene::event(sceneEvent);
		return retval;

		if (true /*retval == false || sceneEvent->isAccepted() == false */)
		{
			switch (sceneEvent->type())
			{
			case QEvent::TouchBegin :
			case QEvent::TouchUpdate :
			case QEvent::TouchEnd :
			case QEvent::TouchCancel :
			{
				//QLOG_INFO() << "Return:" << retval << "Accept:" << sceneEvent->isAccepted();
				emit signalUnacceptedTouchEvent(static_cast<QTouchEvent*>(sceneEvent));
				sceneEvent->accept();
				return true;
			}
			default :
				break;
			}
		}
		return retval;
	}

	Page *SketchScene::page() const
	{
		return _page;
	}
}
