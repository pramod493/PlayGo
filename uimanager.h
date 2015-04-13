#pragma once
#include <QObject>
#include <QAction>
#include <QActionGroup>
#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QToolBar>
#include <QIcon>
#include <QStatusBar>

namespace CDI
{
    class UIManager : public QWidget
    {
        Q_OBJECT
    public:
        //// Customizable variables
        QString title;

        QWidget* parentWidget;

        // TODO - Create action groups in order to simplify interactions as well as menus

        // actions
        QAction* openPageAction;
        QAction* closePageAction;
        QAction* saveImageAction;

        QAction* brushSelectAction;
        QAction* eraseSelectAction;
        QAction* marqueeAction;
        QAction* polygonSelectAction;

        QAction* selectorAction;        // Enable selection of components.
                                        // Touch might make it useless
        QAction* searchAction;          // Search the marqueed region
        QAction* marqueeEnableAction;   // Enable the marquee widget

        QAction *playAction, *pauseAction, *resetAction;

        // Toolbars
        QToolBar* mainToolbar;
        // menus
        QMenuBar* menubar;
        QMenu* editMenu;
        QMenu* helpmenu;

        QGraphicsView* sketchView;          // Primarily use this one. Skip the other ones...
        QGraphicsView* searchView;
        QGraphicsView* gestureView;

        QGraphicsScene* sketchScene;
        QGraphicsScene* searchScene;
        QGraphicsScene* gestureScene;

        QStatusBar* statusBar;

    public:
        /*explicit*/ UIManager(QWidget *parent = 0);

        void CreateActions();

        void SetupToolbar();

        void SetUI();


    signals:

    public slots:
    };
}
