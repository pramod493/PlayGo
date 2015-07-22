#include "sketchscene.h"
#include <QtAlgorithms>
#include <QList>
#include "QsLog.h"
#include "component.h"
#include "stroke.h"
#include "polygon2d.h"
#include "pixmap.h"
#include <QGraphicsWebView>
#include <QUrl>

#define CDI_USE_MOUSE_TO_MOVE

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

	Page *SketchScene::page() const
	{
		return _page;
	}
}
