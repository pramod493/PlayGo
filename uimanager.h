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

namespace CDI
{
    class UIManager : public QObject
    {
        Q_OBJECT
    public:
        // actions
        QAction* openPageAction;
        QAction* closePageAction;
        QAction* saveImageAction;

        QAction* brushSelectAction;
        QAction* eraseSelectAction;
        QAction* lineSelectAction;
        QAction* polygonSelectAction;

        QAction* selectorAction;        // Enable selection of components.
                                        // Touch might make it useless
        QAction* searchAction;          // Search the marqueed region
        QAction* marqueeEnableAction;   // Enable the marquee widget

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




    public:
        /*explicit*/ UIManager(QObject *parent = 0);

        void CreateActions();

        void SetUI(QWidget* widget);


    signals:

    public slots:
    };
}
