// To convert to visual studio, see http://doc.qt.io/qt-5/winrt-support.html
// qmake -tp vc <your project>.pro "CONFIG+=windeployqt"

#include <iostream>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QDateTime>

#include "tabletapplication.h"
#include "cdiwindow.h"
#include "commonfunctions.h"
#include "converttopolygons.h"
#include "QsLog.h"
#include "QsLogDest.h"
#include "filesystemwatcher.h"
#include <QStyleFactory>
#include "batchpolygonize.h"
#include "reconizerwindow.h"

//#include <QGraphicsWebView>
#include <QUrl>
#include <QSsl>
#include <QGraphicsView>
#include <QGraphicsScene>

#ifdef Q_OS_LINUX
#include <qtuio.h>
#endif //Q_OS_LINUX


int main(int argc, char *argv[])
{
//	return grabcut_load("grabcut.png");
	CDI::TabletApplication app(argc, argv);
	/****************************************************************
	 * Set up themes
	 * **************************************************************/
    QStringList list = QStyleFactory::keys();
#ifdef Q_OS_LINUX
	QStyle* style = QStyleFactory::create("Windows");   // Looks more theme independent
	if (style)	app.setStyle(style);
#endif //Q_OS_LINUX

#ifdef Q_OS_WIN
	QStyle* style = QStyleFactory::create("windowsvista");
	if (style)	app.setStyle(style);
#endif Q_OS_WIN

	// Initialize the QsLog logger

	/****************************************************************
	 * Looger initialization
	 * **************************************************************/
	QString qslog_time_format("yyyy-MM-ddThh-mm-ss");
	QString logfilename = QDateTime::currentDateTime().toString(qslog_time_format);
	logfilename += QString(".log");
	// We can also ask for the log file name to be one of the arguments or 
	// pop-up text box for book-keeping the user information
	logfilename = "log.txt";
	// init the logging mechanism
	QsLogging::Logger& logger = QsLogging::Logger::instance();
	logger.setLoggingLevel(QsLogging::TraceLevel);
	const QString sLogPath(QDir(app.applicationDirPath()).filePath(logfilename));
	QsLogging::DestinationPtr fileDestination(
				QsLogging::DestinationFactory::MakeFileDestination(sLogPath) );
	QsLogging::DestinationPtr debugDestination(
				QsLogging::DestinationFactory::MakeDebugOutputDestination() );
	logger.addDestination(debugDestination.get());
	logger.addDestination(fileDestination.get());
	/* Example on how to use QsLog*/

	/****************************************************************
	 * Launch batch polygonizer window
	 * Note that we do not use this info right now
	 * **************************************************************/
	if (false) {
		// This initiates the batch conversion of polygons
		BatchPolygonize* batchpolygonize = new BatchPolygonize;
		batchpolygonize->show();
		return app.exec();
	}

	/****************************************************************
	 * Launch PlayGo main window
	 * **************************************************************/
	CDI::CDIWindow *window =  new CDI::CDIWindow();
	// Triggered when stylus enters/leaves the device proximity
	QObject::connect(&app, SIGNAL(signalStylusEnter()),
                     window, SLOT(onStylusEnter()));

	QObject::connect(&app, SIGNAL(signalStylusLeave()),
					 window, SLOT(onStylusLeave()));

	window->initWidgets();
//	window->showFullScreen();
	window->show();

	/****************************************************************
	 * Initialize TUIO and attach to the sketch view
	 * **************************************************************/
#ifdef Q_OS_LINUX
	if (true)	// Set to true if TUIO is enabled
	{
		QTuio* tuio = new QTuio(window->sketchView);
		tuio->run();	// port: 3333
	}
#endif //Q_OS_LINUX
	/****************************************************************
	 * Launch polygonizer in the main window
	 * **************************************************************/
	//Polygonize p = Polygonize(window);

	/****************************************************************
	 * Launch gesture recognizer window
	 * **************************************************************/
	if (false)
	{
		ReconizerWindow* recognizerWindow =  new ReconizerWindow;
		recognizerWindow->show();
	}

	/******************************************************************
	 *  QT Web View test
	 *****************************************************************/
	/*{
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
	}*/
	return app.exec();
}
