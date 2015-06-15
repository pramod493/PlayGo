#include <iostream>
#include <QDebug>
#include "tabletapplication.h"
#include "cdiwindow.h"
#include "commonfunctions.h"
#include "converttopolygons.h"

int main(int argc, char *argv[])
{
	std::cout << "Testing if it works...";
	CDI::TabletApplication app(argc, argv);
	qDebug() << "Init";
    CDI::CDIWindow *window =  new CDI::CDIWindow();
	// Triggered when stylus enters/leaves the device proximity
	QObject::connect(&app, SIGNAL(OnStylusProximity(QEvent*)),
					 window, SLOT(onStylusProximity(QEvent*)));

	window->initWidgets();
    window->show();
	qDebug() << "Window displayed.";

	Polygonize p = Polygonize(window);
    return app.exec();
}
