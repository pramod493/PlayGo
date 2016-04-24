#include "cdiwindow.h"
#include <QDebug>
#include "QsLog.h"
#include "filesystemwatcher.h"
#include <QMessageBox>
#include <QApplication>
#include <QDockWidget>

namespace CDI
{
	CDIWindow::CDIWindow(QWidget *parent) : QMainWindow(parent)
	{
		playgo = new PlayGo();

//		playgo->ReadModel("PlayGoData.dat");
		Page* newPage = (playgo->currentPage()!= nullptr) ?
					playgo->currentPage() : playgo->addNewPage();
		playgo->setCurrentPage(newPage);

		tabletDevice = QTabletEvent::NoDevice;
	}

	CDIWindow::~CDIWindow()
	{
		if(playgo)
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

		// Create toolbars and related actions
		createActions();
		setupToolbar();
		connectActions();	// All actions are connected to internal function

		// Views and scene
		sketchView = new SketchView(playgo->currentPage(), this);

		// Controller to handle touch, tablet and mouse events
		// and possibly synchronize with Box2D and Indexer
		controller = new PlayGoController(sketchView, this);

		FileSystemWatcher* watcher = new FileSystemWatcher();
#ifdef Q_OS_WIN
		// TODO Always check for directory existence before setting up the watch folder.
		// Do not abuse try-catch for normal usage
		watcher->setDirectory(QString("C:/Database/watched"));
				//watcher->setDirectory(QString("C:/Users/Pramod/Dropbox/Camera Uploads"));
#endif
#ifdef Q_OS_LINUX
//		watcher->setDirectory(QString("~/Dropbox/Camera Uploads"));
#endif
		QObject::connect(watcher, SIGNAL(fileAdded(QString)),
						 controller, SLOT(onExternalImageAdd(QString)));

		// Layout setup
		// Sketch scene and search view are laid out
		// next to each other and separated by a splitter
		splitter = new QSplitter;
		splitter->addWidget(sketchView);	// Do not add any extra widget

		/* Dock not required because search results are displayed is overlaid now
		searchDock = new QDockWidget(tr("Search results"), this);
		searchDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea |
							  Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
		// Removed SearchView
		searchDock->setWidget(searchView);
		addDockWidget(Qt::RightDockWidgetArea, searchDock);*/

		setCentralWidget(splitter);
	}

	void CDIWindow::loadPage(Page *page)
	{
		if (sketchView->getPage() != page)
		{
			sketchView->setPage(page);
			playgo->setCurrentPage(page);
			if (controller) delete controller;
			controller = new PlayGoController(sketchView, this);
		}
	}

	//// Protected functions. Called only for initialization only
	void CDIWindow::createActions()
	{
		// Sketching related operations
		QActionGroup *sketchActions = new QActionGroup(this);

		auto initGroupAction = [&](QString iconname, QString text){
			QAction* tmpAction = new QAction(QIcon(iconname), text, sketchActions);
			tmpAction->setCheckable(true);
			tmpAction->setChecked(false);
			return tmpAction;
		};
		brushSelectAction = initGroupAction(":/images/spline.png", "Draw strokes");
		brushSelectAction->setChecked(true);

		polygonSelectAction	= initGroupAction(":/images/polygon.png", "Sketch polygon");
		eraseSelectAction	= initGroupAction(":/images/eraser.png", "Eraser");
		marqueeAction		= initGroupAction(":/images/marquee.png", "Select");
		connectModeAction	= initGroupAction(":/images/connect.png", "Create joints");

		playAction			= initGroupAction(":/images/play.png", "Play simulation");
		pauseAction			= initGroupAction(":/images/pause.png","Pause simulation");
		resetAction			= initGroupAction(":/images/reset.png", "Reset simulation");

		// Trigger search of either all free strokes or selected strokes
		searchAction		= new QAction(QIcon(":/images/search.png"), tr("Search selection"), this);

		// File opertaions
		newAction			= new QAction(QIcon(":/images/new.png"), tr("New"), this);
		openPageAction		= new QAction(QIcon(":/images/open.png"), tr("Open Page"), this);
		saveImageAction		= new QAction(QIcon(":/images/save.png"), tr("Save as Image"), this);
		cameraLoadAction	= new QAction(QIcon(":/images/Camera.png"), tr("Import from camera"), this);
		closeAction			= new QAction(QIcon(":/images/turn-off.png"), tr("Exit"), this);

		groupAction = new QAction(QIcon(":/images/turn-off.png"), tr("Exit"), this);
		ungroupAction = new QAction(QIcon(":/images/turn-off.png"), tr("Exit"), this);
	}

	void CDIWindow::setupToolbar()
	{
		mainToolbar = new QToolBar(QString(tr("Main toolbar")), this);
		addToolBar(Qt::RightToolBarArea, mainToolbar);

		mainToolbar->addAction(newAction);
		mainToolbar->addAction(openPageAction);
		mainToolbar->addAction(saveImageAction);

		mainToolbar->addAction(closeAction);

		mainToolbar->addSeparator();

		// Create group/ungroup action
		mainToolbar->addAction(groupAction);
		mainToolbar->addAction(ungroupAction);
		mainToolbar->addSeparator();

		mainToolbar->addAction(brushSelectAction);
		mainToolbar->addAction(polygonSelectAction);
		mainToolbar->addAction(connectModeAction);
		mainToolbar->addAction(eraseSelectAction);
		mainToolbar->addAction(marqueeAction);

		mainToolbar->addSeparator();

		mainToolbar->addAction(searchAction);

		mainToolbar->addSeparator();
		mainToolbar->addAction(cameraLoadAction);

		mainToolbar->addSeparator();

		mainToolbar->addAction(playAction);
		mainToolbar->addAction(pauseAction);
		//mainToolbar->addAction(resetAction);	// Not sure what to make of the reset simulation

#ifdef Q_OS_WIN
		mainToolbar->setIconSize(QSize(32,32));
#endif //Q_OS_WIN
#ifdef Q_OS_LINUX
		mainToolbar->setIconSize(QSize(32,32));
#endif // Q_OS_LINUX
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

		QString styleSheet("background-color: #FFFFFF");
		mainToolbar->setStyleSheet(styleSheet);
		colorToolbar->setStyleSheet(styleSheet);
	}

	void CDIWindow::createStatusbar()
	{
		QStatusBar* bar = new QStatusBar(this);
		setStatusBar(bar);
		bar->showMessage(tr("Ready"));
	}

	void CDIWindow::connectActions()
	{
		// New scene.
		connect(newAction, &QAction::triggered,
				this, &CDIWindow::clear);
		// Save
		connect(saveImageAction, &QAction::triggered,
				this, &CDIWindow::save);

		// Alternate approach to connecting with signals
		connect(brushSelectAction, &QAction::triggered,
				[&](){this->setMode(UI::Sketch);});
		connect(polygonSelectAction, &QAction::triggered,
				[&](){this->setMode(UI::Shapes);});
		connect(eraseSelectAction, &QAction::triggered,
				[&](){this->setMode(UI::Erase);});
		connect(connectModeAction, &QAction::triggered,
				[&](){this->setMode(UI::Connect);});
		connect(marqueeAction, &QAction::triggered,
				[&](){this->setMode(UI::Select);});

		// Pen width
		//connect(brushWidthSlider, SIGNAL(valueChanged(int)), this, SLOT(setBrushWidth(int)));
		connect(brushWidthSlider, &QSlider::valueChanged,
				[&](int size){controller->setBrushWidth(size);});

		// Pen color
		connect(colorToolbar, &ColorSelectorToolbar::signalColorChange,
				[&](QString name, QColor col){
			controller->setBrushColor(col);});

		// Search trigger
		connect(searchAction, &QAction::triggered,
				[&](){
			// NOTE We are not using searchDock anymore
			if (!(searchDock==nullptr)) searchDock->setVisible(true);
			controller->searchAction();});

		// Exit program
		connect(closeAction, SIGNAL(triggered()),
				this, SLOT(exit()));

		// Step through simulation
		connect(playAction, &QAction::triggered,
				[&](){controller->startSimulation();});

		// Pause simulation
		connect(pauseAction, &QAction::triggered,
				[&](){controller->pauseSimulation();});

		// Loads the camera widget
		connect(cameraLoadAction, &QAction::triggered,
				[&](){controller->loadCamera();});
	}

	void CDIWindow::closeEvent(QCloseEvent* event)
	{
		// TODO - Trigger save warning
		clear();	// cleanup everything before deleting. Also save
		delete this;
		QMainWindow::closeEvent(event);
	}

	void CDIWindow::clear()
	{
		//TODO This should also prompt to save the current scene.
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(this, "Clear current scene", "Save current scene?",
									  QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
		switch (reply)
		{
		case QMessageBox::Yes :	// Save before deleting
			save();
			controller->clearCurrentScene();
			break;
		case QMessageBox::No :
			controller->clearCurrentScene();
			break;
		case QMessageBox::Cancel :	// Do nothing.
			break;
		}
	}

	void CDIWindow::save()
	{
		if (playgo == nullptr) return;
		playgo->SaveModel("Data.dat");
	}

	void CDIWindow::exit()
	{
		QMessageBox::StandardButton reply;
		reply  = QMessageBox::question(this, "Exit", "Exit?\nAny unsaved changes will be lost.",
									   QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes)
		{
			controller->clearCurrentScene();
			QApplication::quit();
		}
	}

	void CDIWindow::setMode(UI::MODE newmode)
	{
		controller->setMode(newmode);
	}

	void CDIWindow::onStylusEnter()
	{
		if (controller)
			controller->stylusEnter();
	}

	void CDIWindow::onStylusLeave()
	{
		if (controller)
			controller->stylusLeave();
	}
}

