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
#include "QsLog.h"
#include "QsLogDest.h"
#include "filesystemwatcher.h"
#include <QStyleFactory>
#include "reconizerwindow.h"

#include <QSsl>
#include <QGraphicsView>
#include <QGraphicsScene>

#ifdef Q_OS_LINUX
#ifdef ENABLE_TUIO
#include <qtuio.h>
#endif //ENABLE_TUIO
#endif //Q_OS_LINUX

#include "LoadModel.h"

/****************************************************************
 * Set up themes
 * **************************************************************/
void SetupStyles(QApplication& app)
{
	QStringList list = QStyleFactory::keys();
#ifdef Q_OS_LINUX
	QStyle* style = QStyleFactory::create("Windows");   // Looks more theme independent
	if (style)	app.setStyle(style);
#endif //Q_OS_LINUX

#ifdef Q_OS_WIN
	QStyle* style = QStyleFactory::create("windowsvista");
	if (style)	app.setStyle(style);
#endif //Q_OS_WIN
}

void SetupLogger(QApplication& app)
{
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
}

int main(int argc, char *argv[])
{
//	return grabcut_load("grabcut.png");
	CDI::TabletApplication app(argc, argv);

	SetupStyles(app);
	SetupLogger(app);	/***************************************************************
	 * Launch PlayGo main window
	 **************************************************************/
	CDI::CDIWindow *window =  new CDI::CDIWindow();
	// Triggered when stylus enters/leaves the device proximity
	QObject::connect(&app, SIGNAL(signalStylusEnter()),
					 window, SLOT(onStylusEnter()));

	QObject::connect(&app, SIGNAL(signalStylusLeave()),
					 window, SLOT(onStylusLeave()));

	window->initWidgets();
//	window->showFullScreen();
	window->show();

	// Now that we have done loading the workspace, let's set the intial values
	window->brushSelectAction->trigger();
	window->brushWidthSlider->setValue(7);
	return app.exec();
}
