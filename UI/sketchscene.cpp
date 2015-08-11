#include "sketchscene.h"
#include <QtAlgorithms>
#include <QList>
#include "QsLog.h"
#include "scenebutton.h"
//#include <QGraphicsWebView>
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

		/*QPixmap spline = QPixmap(":/images/spline.png");
		QPixmap eraser = QPixmap(":/images/eraser.png");
		QPixmap play   = QPixmap(":/images/joints/lock-joint.png");

		QGraphicsItemGroup *group = new QGraphicsItemGroup;

		SceneButton* btn1 = new SceneButton(spline, QString("Spline"), NULL);	group->addToGroup(btn1);
		SceneButton* btn2 = new SceneButton(eraser, QString("Eraser"), NULL);	group->addToGroup(btn2);
		SceneButton *btn3 = new SceneButton(play, QString("Play"), NULL);		group->addToGroup(btn3);

		addItem(group);
		//group->setFlag(QGraphicsItem::ItemIsMovable);
		group->setFlag(QGraphicsItem::ItemIgnoresTransformations);

		group->setPos(400,400);
		float theta = _PI_ / 4.0f;
		float radius = 250;
		btn1->setPos(radius * cos(theta), radius * sin(theta));
		btn2->setPos(radius * cos(theta*2), radius * sin(theta*2));
		btn3->setPos(radius * cos(theta*3), radius * sin(theta*3));*/

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
		// Delete this section
		// Only for priniting logs
		if (false)
		{
			switch (sceneEvent->type())
			{
			case QEvent::TouchBegin :
			{ qDebug() << "Touch begin scene"; break; }
			case QEvent::TouchUpdate :
			{ qDebug() << "Touch update scene"; break; }
			case QEvent::TouchEnd :
			{ qDebug() << "Touch end scene"; break; }
			case QEvent::TouchCancel :
			{ qDebug() << "Touch cancel scene"; break; }
			case QEvent::TabletPress :
			{ qDebug() << "Tablet press scene"; break; }
			case QEvent::TabletMove :
			{ qDebug() << "Tablet move scene"; break; }
			case QEvent::TabletRelease :
			{ qDebug() << "Tablet release scene"; break; }
			case QEvent::MouseButtonDblClick :
			{ qDebug() << "Mouse button double click scene"; break; }
			case QEvent::MouseButtonPress :
			{ qDebug() << "Mouse button press scene"; break; }
			case QEvent::MouseButtonRelease :
			{ qDebug() << "Mouse button release scene"; break; }
			case QEvent::MouseMove :
			{qDebug() << "Mouse move scene"; break;	}
			}
			sceneEvent->accept();
			return true;
		}
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
				//qDebug() << "Return:" << retval << "Accept:" << sceneEvent->isAccepted();
				emit signalUnacceptedTouchEvent(static_cast<QTouchEvent*>(sceneEvent));
				sceneEvent->accept();
				return true;
			}
			case QEvent::TabletPress :
			case QEvent::TabletMove :
			case QEvent::TabletRelease :
			{
				sceneEvent->accept();
				return true;
			}
			case QEvent::MouseButtonDblClick :
			case QEvent::MouseButtonPress :
			case QEvent::MouseButtonRelease :
			case QEvent::MouseMove :
			{
				sceneEvent->accept();
				return true;
			}
			}
		}
		return retval;
	}

	Page *SketchScene::page() const
	{
		return _page;
	}
}
