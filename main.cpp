#include <QApplication>
#include <QMainWindow>
#include "tabletapplication.h"
#include "cdiwindow.h"
#include "uimanager.h"

int main(int argc, char *argv[])
{
    CDI::TabletApplication app(argc, argv);

    CDI::CDIWindow *window =  new CDI::CDIWindow();

	window->InitWidgets();

    window->show();

    return app.exec();
}
