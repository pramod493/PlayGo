#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsWebView>
#include <QWidget>
#include <QUrl>

/******************************************************************
*  QT Web View test
*****************************************************************/
void DemoWebView() 
{
	QGraphicsWebView *webwidget = new QGraphicsWebView;
	QUrl url("http://www.google.com");
	webwidget->load(url);
	QGraphicsView* view = new QGraphicsView();
	QGraphicsScene* scene = new QGraphicsScene(view);
	view->setScene(scene);
	view->show();
	//webwidget->setFlag(QGraphicsItem::ItemIsMovable, true);
	//window->sketchView->scene()->addItem(webwidget);
	scene->addItem(webwidget);
}