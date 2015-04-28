#include <QApplication>
#include <QMainWindow>
#include "tabletapplication.h"
#include "cdiwindow.h"

int main(int argc, char *argv[])
{
    CDI::TabletApplication app(argc, argv);

    CDI::CDIWindow *window =  new CDI::CDIWindow();

	QObject::connect(&app, SIGNAL(OnStylusProximity(QEvent*)),
					 window, SLOT(slotOnSignalProximity(QEvent*)));

	window->InitWidgets();

    window->show();

    return app.exec();
}
