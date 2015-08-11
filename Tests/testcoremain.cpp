#include <QApplication>
#include "playgocore.h"
#include <QDebug>
#include "playgo.h"
using namespace CDI;

int main (int argc, char* argv[])
{
    QApplication app(argc, argv);

    qDebug() << "init main core test";
//    PlayGo *playgo = new PlayGo();
//    Page* page = playgo->addNewPage();
//    playgo->setCurrentPage(page);

//    Component*  component = page->createComponent();
//    component->addStroke(Qt::green, 2.0f);
    return app.exec();
}
