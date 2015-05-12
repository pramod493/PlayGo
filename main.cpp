#include <QApplication>
#include <QMainWindow>
#include "tabletapplication.h"
#include "cdiwindow.h"

#include "wbbice.h"
#include "wbsearchengine.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>


int main(int argc, char *argv[])
{
    CDI::TabletApplication app(argc, argv);

    CDI::CDIWindow *window =  new CDI::CDIWindow();

	QObject::connect(&app, SIGNAL(OnStylusProximity(QEvent*)),
					 window, SLOT(slotOnSignalProximity(QEvent*)));

	window->InitWidgets();
    window->show();

    /*{
        namespace po = boost::program_options;
        // Uncomment to test if the search is working or not.
        std::string file = "C:/Database/Input.png";
        path data("C:/Database/");
        wbBICE *bicedescriptor = new wbBICE();
        wbSearchEngine *engine = new wbSearchEngine(data, bicedescriptor);

        engine->Index();
        engine->Load();
        engine->Query(file, 40);
    }*/
    return app.exec();
}
