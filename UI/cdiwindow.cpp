#include "cdiwindow.h"
#include <QDebug>
#include <QGraphicsProxyWidget>
#include <QColorDialog>
#include "colorselectortoolbar.h"
#include <QDir>
#include <QFile>
#include <QTextStream>

namespace CDI
{
    CDIWindow::CDIWindow(QWidget *parent) : QMainWindow(parent)
    {
        playgo = new PlayGo();

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
        ColorSelectorToolbar* colorToolbar = new ColorSelectorToolbar();
        colorToolbar->InitToolbarItems();
        colorToolbar->setOrientation(Qt::Vertical);
        colorToolbar->setFloatable(true);
        colorToolbar->setMovable(true);
        insertToolBar(mainToolbar,colorToolbar);
        colorToolbar->show();

        ////
        mainLayout = new QVBoxLayout;
        sketchView = new SketchView(this);
        sketchScene = new SketchScene(sketchView);
        sketchView->setScene(sketchScene);
        sketchView->setSceneRect(0,0,5000,5000);
        sketchView->setRenderHint(QPainter::Antialiasing);
        sketchView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

        if (0){
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
            //// Crashes the program
            //            QColorDialog *_dialog = new QColorDialog();
            //            _dialog->setOption(QColorDialog::NoButtons, true);
            //            QGraphicsProxyWidget *proxyWidget = new QGraphicsProxyWidget();
            //            proxyWidget->setWidget(_dialog);
            //            sketchScene->addWidget(_dialog);
            //            sketchScene->setSceneRect(_dialog->geometry());
        }

        // Tablet event
        connect(sketchView, SIGNAL(signalViewTabletEvent(QTabletEvent*, QPointF)),
                sketchScene, SLOT(slotTabletEvent(QTabletEvent*, QPointF)));
        // NOTE - make sure that actions are intialized prior to this segment

        // Save
        connect(saveImageAction, SIGNAL(triggered()),
                this, SLOT(Save()));
        // Draw
        connect(brushSelectAction, SIGNAL(triggered()),
                sketchScene,SLOT(setToDraw()));
        // Erase
        connect(eraseSelectAction, SIGNAL(triggered()),
                sketchScene,SLOT(setToErase()));
        // Select
        connect(marqueeAction, SIGNAL(triggered()),
                sketchScene,SLOT(setToSelect()));
        // Brush width
        connect(this, SIGNAL(signalBrushSizeChanged(int)),
                sketchScene, SLOT(setBrushWidth(int)));
        brushWidthSlider->setValue(3);
        connect(colorToolbar, SIGNAL(signalColorChange(QString,QColor)),
                sketchScene, SLOT(setBrushColor(QString,QColor)));

        // Search
        connect(searchAction, SIGNAL(triggered()),
                sketchScene, SLOT(OnSearchTrigger()));

        setCentralWidget(sketchView);
    }

    //// Protected functions. Called only for initialization only
    void CDIWindow::CreateActions()
    {
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

        mainToolbar->setIconSize(QSize(64,64));

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

    void CDIWindow::Save()
    {
		// Not needed as we are QDataStream
//        if (sketchScene!= NULL)
//        {
//            QTime time = QTime::currentTime();

//            QString filePath = QDir::currentPath() + "/" + time.toString("hh:mm:ss.zzz").replace(QRegExp(":"), "_");
//            qDebug() << "File" << filePath;
//            QFile file(filePath);
//            file.open(QIODevice::WriteOnly);
//            QTextStream out(&file);
//            out << "#Saving to file at "<< filePath;
//            {
////                sketchScene
////                sketchView
//                foreach (GraphicsPathItem* item, sketchScene->freeStrokes)
//                {
//                    out << "@POINT2DPT" << "\n";
//					foreach (Point2DPT* point, item->parentStroke)
//                    {
//                        out << point->x << " " << point->y << " "
//                            << point->pressure << " " << point->time << "\n";
//                    }
//                }
//            }
//            file.close();

//        }
    }
}
