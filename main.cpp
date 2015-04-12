#include <QApplication>
#include <QMainWindow>
#include "cdiwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    CDI::CDIWindow *window =  new CDI::CDIWindow();
    window->show();

    return a.exec();
}
