#include <QApplication>
#include <QMainWindow>
#include "tabletapplication.h"
#include "cdiwindow.h"

#include "QPixmap"
#include "QBitmap"

int main(int argc, char** argv)
{
	CDI::TabletApplication app(argc, argv);
	CDI::CDIWindow * window = new CDI::CDIWindow();
	
	QObject::connect(&app, SIGNAL(OnStylusProximity(QEvent*)),
					window, SLOT(slotOnSignalProximity(QEvent*)));
	window->InitWidgets();
	window->show();
	
	return app.exec();				
}

void testPixmapMasking()
{
	// Testing masking operations
	QPixmap pixmap = QPixmap();
	pixmap.load("E:/Camera.png");
	qDebug() <<"Image loaded with " <<pixmap.size();
	pixmap.createHeuristicMask(true);
	QBitmap maskmap = pixmap.mask();
	maskmap.save("E:/maskCar.png");
}

void testSearchEngineInitialization () 
{
    namespace po = boost::program_options;
    // Uncomment to test if the search is working or not.
    std::string file = "C:/Database/Input.png";
    path data("C:/Database/");
    wbBICE *bicedescriptor = new wbBICE();
    wbSearchEngine *engine = new wbSearchEngine(data, bicedescriptor);

    engine->Index();
    engine->Load();
    engine->Query(file, 40);
}

void testTransorm()
{
	// Test out transform stuff
	QTransform t;
	qDebug() << t;
	t.translate(3,5);
	t.rotate(90);
	qDebug() << t;
	QPointF p1(10,11);
	qDebug() << p1 << "mapped becomes" << p1*t;
}