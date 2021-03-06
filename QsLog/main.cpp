#include "QsLog.h"
#include "QsLogDest.h"
#include <QtCore/QCoreApplication>
#include <QDir>
#include <iostream>
#include <QDateTime>

// Using this to save the file name and therefore need not
// have ms as well as must not contain ":" else file won't open on windows
const QString qslog_time_format("yyyy-MM-ddThh-mm-ss");
int main(int argc, char *argv[])
{
   QCoreApplication a(argc, argv);

   QString logfilename = QDateTime::currentDateTime().toString(qslog_time_format);
   logfilename += QString(".log");
   // init the logging mechanism
   QsLogging::Logger& logger = QsLogging::Logger::instance();
   logger.setLoggingLevel(QsLogging::TraceLevel);
   const QString sLogPath(QDir(a.applicationDirPath()).filePath(logfilename));
   QsLogging::DestinationPtr fileDestination(
      QsLogging::DestinationFactory::MakeFileDestination(sLogPath) );
   QsLogging::DestinationPtr debugDestination(
      QsLogging::DestinationFactory::MakeDebugOutputDestination() );
   logger.addDestination(debugDestination.get());
   logger.addDestination(fileDestination.get());
   //logger.setLoggingLevel(QsLogging::InfoLevel);

   QLOG_INFO() << "Program started";
   QLOG_INFO() << "Built with Qt" << QT_VERSION_STR << "running on" << qVersion();

   QLOG_TRACE() << "Here's a" << QString("trace") << "message";
   QLOG_DEBUG() << "Here's a" << static_cast<int>(QsLogging::DebugLevel) << "message";
   QLOG_WARN()  << "Uh-oh!";
   qDebug() << "This message won't be picked up by the logger";
   QLOG_ERROR() << "An error has occurred";
   qWarning() << "Neither will this one";
   QLOG_FATAL() << "Fatal error!";

   const int ret = 0;
   std::cout << std::endl << "Press any key...";
   std::cin.get();
   QLOG_INFO() << "Program exited with return code" << ret;
   return ret;
}
