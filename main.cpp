// To convert to visual studio, see http://doc.qt.io/qt-5/winrt-support.html
// qmake -tp vc <your project>.pro "CONFIG+=windeployqt"

#include <iostream>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QDateTime>

#include <QSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>

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

#include "qCustomPlot/qcustomplot.h"

#include "LoadModel.h"

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
	 * Test QCustomPlot basic plotting
	 * (Only for testing purposes)
	 * **************************************************************/
	if (false) {

		QCustomPlot* customPlot = new QCustomPlot();
		// Plot 01
		{
			// generate some data:
			QVector<double> x(101), y(101); // initialize with entries 0..100
			for (int i=0; i<101; ++i)
			{
				x[i] = i/50.0 - 1; // x goes from -1 to 1
				y[i] = x[i]*x[i]; // let's plot a quadratic function
			}
			// create graph and assign data to it:
			customPlot->addGraph();
			customPlot->graph(0)->setData(x, y);
			// give the axes some labels:
			customPlot->xAxis->setLabel("x");
			customPlot->yAxis->setLabel("y");
			// set axes ranges, so we see all data:
			customPlot->xAxis->setRange(-1, 1);
			customPlot->yAxis->setRange(0, 1);
			customPlot->replot();
			customPlot->show();
		}
		// Plot 02
		{
			// add two new graphs and set their look:
			customPlot->addGraph();
			customPlot->graph(0)->setPen(QPen(Qt::blue)); // line color blue for first graph
			customPlot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20))); // first graph will be filled with translucent blue
			customPlot->addGraph();
			customPlot->graph(1)->setPen(QPen(Qt::red)); // line color red for second graph
			// generate some points of data (y0 for first, y1 for second graph):
			QVector<double> x(250), y0(250), y1(250);
			for (int i=0; i<250; ++i)
			{
				x[i] = i;
				y0[i] = exp(-i/150.0)*cos(i/10.0); // exponentially decaying cosine
				y1[i] = exp(-i/150.0); // exponential envelope
			}
			// configure right and top axis to show ticks but no labels:
			// (see QCPAxisRect::setupFullAxesBox for a quicker method to do this)
			customPlot->xAxis2->setVisible(true);
			customPlot->xAxis2->setTickLabels(false);
			customPlot->yAxis2->setVisible(true);
			customPlot->yAxis2->setTickLabels(false);
			// make left and bottom axes always transfer their ranges to right and top axes:
			QObject::connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
			QObject::connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
			// pass data points to graphs:
			customPlot->graph(0)->setData(x, y0);
			customPlot->graph(1)->setData(x, y1);
			// let the ranges scale themselves so graph 0 fits perfectly in the visible area:
			customPlot->graph(0)->rescaleAxes();
			// same thing for graph 1, but only enlarge ranges (in case graph 1 is smaller than graph 0):
			customPlot->graph(1)->rescaleAxes(true);
			// Note: we could have also just called customPlot->rescaleAxes(); instead
			// Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
			customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
		}
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

	// Now that we have done loading the workspace, let's set the intial values
	window->brushSelectAction->trigger();
	window->brushWidthSlider->setValue(7);

	/****************************************************************
	 * Initialize TUIO and attach to the sketch view
	 * **************************************************************/
#ifdef Q_OS_LINUX
	bool tuio_enabled = false;
	if (tuio_enabled)	// Set to true if TUIO is enabled
	{
		// default port is 3333. Use multiple ports for multi-screen
		QTuio* tuio = new QTuio(window->sketchView);
		tuio->run();
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

	/******************************************************************
	 *  QT Web View test
	 *****************************************************************/
	{
		LoadModel* test = new LoadModel(window->controller);
		QObject::connect(window->saveImageAction, SIGNAL(triggered()),
						 test, SLOT(runTest()));
	}
	/******************************************************************
	 *  Sql test (works)
	 *****************************************************************/
	if (false) {
		QSqlDatabase db;
		db = QSqlDatabase::addDatabase("QSQLITE");
		db.setDatabaseName("PlayGoDataBase.db3");
		db.open();

		QSqlQuery q("", db);
//		q.exec("drop table Movies");
		q.exec("drop table Names");
		q.exec("drop table groceries");
//		q.exec("create table Movies (id integer primary key, Title varchar, Director varchar, Rating number)");
//		q.exec("insert into Movies values (0, 'Metropolis', 'Fritz Lang', '8.4')");
//		q.exec("insert into Movies values (1, 'Nosferatu, eine Symphonie des Grauens', 'F.W. Murnau', '8.1')");
//		q.exec("insert into Movies values (2, 'Bis ans Ende der Welt', 'Wim Wenders', '6.5')");
//		q.exec("insert into Movies values (3, 'Hardware', 'Richard Stanley', '5.2')");
//		q.exec("insert into Movies values (4, 'Mitchell', 'Andrew V. McLaglen', '2.1')");
		q.exec("create table Names (id integer primary key, Firstname varchar, Lastname varchar, City varchar)");
		q.exec("insert into Names values (0, 'Sala', 'Palmer', 'Morristown')");
		q.exec("insert into Names values (1, 'Christopher', 'Walker', 'Morristown')");
		q.exec("insert into Names values (2, 'Donald', 'Duck', 'Andeby')");
		q.exec("insert into Names values (3, 'Buck', 'Rogers', 'Paris')");
		q.exec("insert into Names values (4, 'Sherlock', 'Holmes', 'London')");
		q.exec("insert into Names values (9, 'Buck', 'Rogers', 'Paris')");
		q.exec("insert into Names values (19, 'Sherlock', 'Holmes', 'London')");
		q.exec("insert into Names values (7, 'Leonardo', 'Caprio', 'America')");
		q.exec("insert into Names values (8, 'No Oscar', 'Caprio', 'America')");
		q.exec("create table groceries (id integer primary key, name varchar, quantity number, price number)");
		q.exec("insert into groceries values (1, 'banana', '10', '1.56')");
		q.exec("insert into groceries values (2, 'apple', '14', '4.56')");
		db.close();
	}
	return app.exec();
}
