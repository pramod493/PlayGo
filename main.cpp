#include <iostream>
#include <QDebug>
#include <QDir>
#include <QFile>

#include "tabletapplication.h"
#include "cdiwindow.h"
#include "commonfunctions.h"
#include "converttopolygons.h"
#include "QsLog.h"
#include "QsLogDest.h"
int grabcut_load(string outputfile);

int main(int argc, char *argv[])
{
//	return grabcut_load("grabcut.png");
    CDI::TabletApplication app(argc, argv);
    // Initialize the QsLog logger

	QString qslog_time_format("yyyy-MM-ddThh-mm-ss");
	QString logfilename = QDateTime::currentDateTime().toString(qslog_time_format);
	logfilename += QString(".log");
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

    CDI::CDIWindow *window =  new CDI::CDIWindow();

    // Triggered when stylus enters/leaves the device proximity
	QObject::connect(&app, SIGNAL(OnStylusProximity(QEvent*)),
					 window, SLOT(onStylusProximity(QEvent*)));

	window->initWidgets();
    window->show();

	//Polygonize p = Polygonize(window);
    return app.exec();
}
