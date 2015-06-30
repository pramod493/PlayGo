#include "cdiwindow.h"
#include <QDebug>
#include <QGraphicsProxyWidget>
#include <QColorDialog>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QPushButton>
#include <QSplitter>
#include "playgocore.h"
#include "searchview.h"

namespace CDI
{
	CDIWindow::CDIWindow(QWidget *parent) : QMainWindow(parent)
	{
		playgo = new PlayGo();

		playgo->ReadModel("PlayGoData.dat");
		Page* newPage = NULL;
		if (playgo->currentPage()!= NULL)
		{
			newPage = playgo->currentPage();
		} else
		{
			playgo->addNewPage();
			newPage = playgo->addNewPage();
		}
		playgo->setCurrentPage(newPage);

		tabletDevice = QTabletEvent::NoDevice;
	}

	CDIWindow::~CDIWindow()
	{
		if(playgo!= NULL)
		{
			playgo->SaveModel("PlayGoData.dat");
		}
	}

	void CDIWindow::initWidgets()
	{
		// Set up the main window propoerties
		setObjectName(QStringLiteral("Main Sketch Window"));
		setWindowTitle("CDI Sketcher - PlayGo");

		// Window size policies
		QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		sizePolicy.setHorizontalStretch(0);
		sizePolicy.setVerticalStretch(0);
		setSizePolicy(sizePolicy);

		createActions();
		setupToolbar();
		connectActions();

		sketchView = new SketchView(playgo->currentPage(), this);
		sketchScene = sketchView->getSketchScene();
//		SearchView* searchView = new SearchView(this);
		controller = new PlayGoController(sketchScene, sketchView, this);

//		QHBoxLayout* boxlayout = new QHBoxLayout;
//		QSplitter *split = new QSplitter;
//		split->addWidget(sketchView);
//		split->addWidget(searchView);
//		boxlayout->addWidget(split);

//		setLayout(boxlayout);

		setCentralWidget(sketchView);
	}

	//// Protected functions. Called only for initialization only
	void CDIWindow::createActions()
	{
		// Sketching related operations
		QActionGroup *sketchActions = new QActionGroup(this);

		brushSelectAction = new QAction(QIcon(":/images/spline.png"), tr("Draw strokes"), sketchActions);
		brushSelectAction->setCheckable(true);
		brushSelectAction->setChecked(true);

		eraseSelectAction = new QAction(QIcon(":/images/eraser.png"), tr("Eraser"), sketchActions);
		eraseSelectAction->setCheckable(true);
		eraseSelectAction->setChecked(false);

		marqueeAction = new QAction(QIcon(":/images/marquee.png"), tr("Select"), sketchActions);
		marqueeAction->setCheckable(true);
		marqueeAction->setChecked(false);

		// Physics simulation related actions
		QActionGroup *playActions = new QActionGroup(this);

		playAction = new QAction(QIcon(":/images/play.png"), tr("Play simulation"), playActions);
		playAction->setCheckable(true);
		playAction->setChecked(false);

		pauseAction = new QAction(QIcon(":/images/pause.png"), tr("Pause simulation"), playActions);
		pauseAction->setCheckable(true);
		playAction->setChecked(false);

		resetAction = new QAction(QIcon(":/images/reset.png"), tr("Reset simulation"), playActions);
		pauseAction->setCheckable(true);
		pauseAction->setChecked(false);

		// Trigger search of either all free strokes or selected strokes
		searchAction = new QAction(QIcon(":/images/search.png"), tr("Search selection"), this);

		// File opertaions
		newAction = new QAction(QIcon(":/images/new.png"), tr("New"), this);
		openPageAction = new QAction(QIcon(":/images/open.png"), tr("Open Page"), this);
		saveImageAction = new QAction(QIcon(":/images/save.png"), tr("Save as Image"), this);
	}

	void CDIWindow::setupToolbar()
	{
		mainToolbar = new QToolBar(QString(tr("Main toolbar")), this);
		addToolBar(Qt::RightToolBarArea, mainToolbar);

		mainToolbar->addAction(newAction);
		mainToolbar->addAction(openPageAction);
		mainToolbar->addAction(saveImageAction);

		mainToolbar->addSeparator();

		mainToolbar->addAction(brushSelectAction);
		mainToolbar->addAction(eraseSelectAction);
		mainToolbar->addAction(marqueeAction);

		mainToolbar->addSeparator();

		mainToolbar->addAction(searchAction);

		mainToolbar->addSeparator();

		mainToolbar->addAction(playAction);
		mainToolbar->addAction(pauseAction);
		mainToolbar->addAction(resetAction);

        mainToolbar->setIconSize(QSize(32,32));

		brushWidthSlider = new QSlider(Qt::Horizontal);
		brushWidthSlider->setRange(2,20);
		brushWidthSlider->setSingleStep(1);

		mainToolbar->setFloatable(true);
		mainToolbar->setMovable(true);
		mainToolbar->addWidget(brushWidthSlider);
		mainToolbar->show();

		// Create a color selector toolbar
		colorToolbar = new ColorSelectorToolbar();
		colorToolbar->InitToolbarItems();
		addToolBar(Qt::LeftToolBarArea, colorToolbar);
		colorToolbar->show();
	}

	void CDIWindow::createStatusbar()
	{
		QStatusBar* bar = new QStatusBar(this);
		setStatusBar(bar);
		bar->showMessage(tr("Ready"));
	}

	void CDIWindow::connectActions()
	{
		// New
        connect(newAction, SIGNAL(triggered()),
				this, SLOT(clear()));
		// Save
		connect(saveImageAction, SIGNAL(triggered()),
				this, SLOT(save()));
		// Draw
		connect(brushSelectAction, SIGNAL(triggered()),
				this,SLOT(setToDraw()));
		// Erase
		connect(eraseSelectAction, SIGNAL(triggered()),
				this,SLOT(setToErase()));
		// Select
		connect(marqueeAction, SIGNAL(triggered()),
				this,SLOT(setToSelect()));
		// Pen width
		connect(brushWidthSlider, SIGNAL(valueChanged(int)),
				this, SLOT(setBrushWidth(int)));
		// Pen color
		connect(colorToolbar, SIGNAL(signalColorChange(QString,QColor)),
				this, SLOT(setBrushColor(QString,QColor)));
		// Search trigger
		connect(searchAction, SIGNAL(triggered()),
				this, SLOT(onSearchTrigger()));

		brushWidthSlider->setValue(6);
	}

	void CDIWindow::closeEvent(QCloseEvent* event)
	{
		// TODO - Trigger save warning
		save();
		delete this;
		QMainWindow::closeEvent(event);
	}

	void CDIWindow::clear()
	{
        sketchScene->clear();
		sketchScene->update();
        controller->clearCurrentScene();
	}

	void CDIWindow::save()
	{
		if(playgo!= NULL)
		{
			playgo->SaveModel("PlayGoData.dat");
		}
	}

	void CDIWindow::setToDraw()
	{
		controller->setToDraw();
	}

	void CDIWindow::setToErase()
	{
		controller->setToErase();
	}

	void CDIWindow::setToSelect()
	{
		controller->setToSelect();
	}

	void CDIWindow::setBrushWidth(int size)
	{
		controller->setBrushWidth(size);
	}

	void CDIWindow::setBrushColor(QString name, QColor col)
	{
		controller->setBrushColor(col);
	}

	void CDIWindow::onSearchTrigger()
	{
		qDebug() << "Search selection";
		if (controller!= NULL)
		{
			controller->searchAction();
		}
	}

	void CDIWindow::onStylusProximity(QEvent* event)
	{
		tabletDevice = static_cast<QTabletEvent*>(event)->device();
		if (tabletDevice == QTabletEvent::Stylus)
			controller->enableMouse(false);
	}
}
/*
 *
		if (0) {
			//             Testing out duplicate scenes. We can show same scene in multiple views.
			// Zoom works too
			QGraphicsView* view = new QGraphicsView();
			view->setScene(sketchScene);
			view->fitInView(0,0,1000,1000,Qt::KeepAspectRatio);
			view->show();

			view->setAttribute(Qt::WA_AcceptTouchEvents, true);
			QLinearGradient gradient = QLinearGradient(0,0,0,1000);
			gradient.setColorAt(0.0,QColor(255,175,175,100));
			gradient.setColorAt(1.0, QColor(200,200,200,255));
			view->setBackgroundBrush(QBrush(gradient));
		}
		*/
