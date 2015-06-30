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


int main(int argc, char *argv[])
{
    CDI::TabletApplication app(argc, argv);
    // Initialize the QsLog logger
    {
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
        /* Example on how to use QsLog
           QLOG_INFO() << "Program started";
           QLOG_INFO() << "Built with Qt" << QT_VERSION_STR << "running on" << qVersion();

           QLOG_TRACE() << "Here's a" << QString("trace") << "message";
           QLOG_DEBUG() << "Here's a" << static_cast<int>(QsLogging::DebugLevel) << "message";
           QLOG_WARN()  << "Uh-oh!";
           qDebug() << "This message won't be picked up by the logger";
           QLOG_ERROR() << "An error has occurred";
           qWarning() << "Neither will this one";
           QLOG_FATAL() << "Fatal error!";
         * */
    }

    CDI::CDIWindow *window =  new CDI::CDIWindow();

    // Triggered when stylus enters/leaves the device proximity
	QObject::connect(&app, SIGNAL(OnStylusProximity(QEvent*)),
					 window, SLOT(onStylusProximity(QEvent*)));

	window->initWidgets();
    window->show();

	//Polygonize p = Polygonize(window);
    return app.exec();
}
