#include "sketchscene.h"
#include <QtAlgorithms>
#include <QList>
#include "QsLog.h"
#include "scenebutton.h"
#include <QGraphicsWebView>
#include <QGraphicsItemGroup>
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

		QPixmap spline = QPixmap(":/images/spline.png");
		QPixmap eraser = QPixmap(":/images/eraser.png");
		QPixmap play   = QPixmap(":/images/joints/lock-joint.png");

		QGraphicsItemGroup *group = new QGraphicsItemGroup;

//		SceneButton* btn1 = new SceneButton(spline, QString("Spline"), NULL);	group->addToGroup(btn1);
//		SceneButton* btn2 = new SceneButton(eraser, QString("Eraser"), NULL);	group->addToGroup(btn2);
//		SceneButton *btn3 = new SceneButton(play, QString("Play"), NULL);		group->addToGroup(btn3);

//		addItem(group);
//		//group->setFlag(QGraphicsItem::ItemIsMovable);
//		group->setFlag(QGraphicsItem::ItemIgnoresTransformations);

//		group->setPos(400,400);
//		float theta = _PI_ / 4.0f;
//		float radius = 250;
//		btn1->setPos(radius * cos(theta), radius * sin(theta));
//		btn2->setPos(radius * cos(theta*2), radius * sin(theta*2));
//		btn3->setPos(radius * cos(theta*3), radius * sin(theta*3));

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
