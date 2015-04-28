#include "cdiwindow.h"

namespace CDI
{
	CDIWindow::CDIWindow(QWidget *parent) : QMainWindow(parent)
	{
		tabletDevice = QTabletEvent::NoDevice;
	}

	CDIWindow::~CDIWindow()
	{

	}

	void CDIWindow::InitWidgets()
	{
		// Set up the main window propoerties
		setObjectName(QStringLiteral("Main Sketch Window"));
		setWindowTitle("CDI Sketcher - PlayGo");

		//setCentralWidget();

		QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		sizePolicy.setHorizontalStretch(0);
		setSizePolicy(sizePolicy);

		// Initialize UI elements
		CreateActions();
		SetupToolbar();

		////
		mainLayout = new QVBoxLayout;
		sketchView = new SketchView(this);
		sketchScene = new SketchScene(sketchView);
		sketchView->setScene(sketchScene);
		sketchView->setSceneRect(0,0,sketchView->width(),sketchView->height());
		sketchView->setRenderHint(QPainter::Antialiasing);
		sketchView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

		connect(sketchView, SIGNAL(signalViewTabletEvent(QTabletEvent*, QPointF)),
				sketchScene, SLOT(slotTabletEvent(QTabletEvent*, QPointF)));

		setCentralWidget(sketchView);
	}

	//// Protected functions. Called only for initialization only
	void CDIWindow::CreateActions()
	{
		QActionGroup *sketchActions = new QActionGroup(this);

		brushSelectAction = new QAction(QIcon(":/images/spline.png"), tr("Draw strokes"), sketchActions);
		brushSelectAction->setCheckable(true);
		brushSelectAction->setChecked(false);

		eraseSelectAction = new QAction(QIcon(":/images/eraser.png"), tr("Eraser"), sketchActions);
		eraseSelectAction->setCheckable(true);
		eraseSelectAction->setChecked(false);

		marqueeAction = new QAction(QIcon(":/images/marquee.png"), tr("Select"), sketchActions);
		marqueeAction->setCheckable(true);
		marqueeAction->setChecked(false);

		QActionGroup *playActions = new QActionGroup(this);

		playAction = new QAction(QIcon(":/images/play.png"), tr("Play simulation"), playActions);
		playAction->setCheckable(true);
		playAction->setChecked(false);

		pauseAction = new QAction(QIcon(":/images/pause.png"), tr("Pause simulation"), playActions);
		pauseAction->setCheckable(true);

		resetAction = new QAction(QIcon(":/images/reset.png"), tr("Reset simulation"), playActions);
		pauseAction->setCheckable(true);

		searchAction = new QAction(QIcon(":/images/search.png"), tr("Search selection"), this);

		//// File opertaions
		openPageAction = new QAction(QIcon(":/images/open.png"), tr("Open Page"), this);
		saveImageAction = new QAction(QIcon(":/images/save.png"), tr("Save as Image"), this);
	}

	void CDIWindow::SetupToolbar()
	{
		//		mainToolbar = new QToolBar(this);
		mainToolbar = addToolBar(tr("Main operations"));

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

		mainToolbar->setIconSize(QSize(96,96));

		brushWidthSlider = new QSlider(Qt::Horizontal);
		brushWidthSlider->setRange(2,20);
		brushWidthSlider->setSingleStep(1);
		connect(brushWidthSlider, SIGNAL(valueChanged(int)),
				this, SLOT(setBrushValue(int)));
		mainToolbar->addWidget(brushWidthSlider);

		mainToolbar->show();
	}

	void CDIWindow::CreateStatusbar()
	{
		QStatusBar* bar = new QStatusBar(this);
		setStatusBar(bar);
		bar->showMessage(tr("Ready"));
	}

	void CDIWindow::closeEvent(QCloseEvent* event)
	{
		// TODO - Trigger save warning
		QMainWindow::closeEvent(event);
	}

	void CDIWindow::setBrushValue(int size)
	{
		emit signalBrushSizeChanged(size);
	}

	void CDIWindow::slotOnSignalProximity(QEvent* event)
	{
		tabletDevice = static_cast<QTabletEvent*>(event)->device();
	}
}
