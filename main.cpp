#include <QApplication>
#include <QMainWindow>
#include "tabletapplication.h"
#include "cdiwindow.h"
#include <QTransform>
#include <QDebug>
#include "commonfunctions.h"

void testPoly2Tri();

int main(int argc, char *argv[])
{
    CDI::TabletApplication app(argc, argv);

//	{
//		unsigned int mask =0;
//		qDebug() << mask;
//		mask |= MASK_BIT2;
//		mask |= MASK_BIT0;
//		mask |= MASK_BIT1;
//		qDebug() << mask;

//		qDebug() << (mask &= ~MASK_BIT1);	// use this

//		return 1;
//	}

    CDI::CDIWindow *window =  new CDI::CDIWindow();

	QObject::connect(&app, SIGNAL(OnStylusProximity(QEvent*)),
					 window, SLOT(slotOnSignalProximity(QEvent*)));


	window->InitWidgets();
    window->show();

    return app.exec();
}
