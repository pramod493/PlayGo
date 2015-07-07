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

int main(int argc, char *argv[])
{
//	return grabcut_load("grabcut.png");
    CDI::TabletApplication app(argc, argv);
#ifdef Q_OS_WIN
	//QStringList list = QStyleFactory::keys();
	QStyle* style = QStyleFactory::create("Fusion");
	if (style)
		app.setStyle(style);
	// Initialize the QsLog logger
#endif //Q_OS_WIN


	QString qslog_time_format("yyyy-MM-ddThh-mm-ss");
	QString logfilename = QDateTime::currentDateTime().toString(qslog_time_format);
	logfilename += QString(".log");
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

	if (false) {
		// This initiates the batch conversion of polygons
		BatchPolygonize* batchpolygonize = new BatchPolygonize;
		batchpolygonize->show();
		return app.exec();
	}
	CDI::CDIWindow *window =  new CDI::CDIWindow();
    // Triggered when stylus enters/leaves the device proximity
	QObject::connect(&app, SIGNAL(OnStylusProximity(QEvent*)),
					 window, SLOT(onStylusProximity(QEvent*)));

	window->initWidgets();
	window->showFullScreen();
	window->show();

	//PolygoqDebug() << CDI::currentTime();nize p = Polygonize(window);
    return app.exec();
}
