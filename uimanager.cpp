#include "uimanager.h"
#include <QObject>
#include <QActionGroup>
#include <QWidget>
namespace CDI {


    UIManager::UIManager(QWidget *parent)
        : QWidget(parent)
    {
        title = "PlayGo";
        parentWidget = parent;
    }

    void UIManager::CreateActions()
    {
		QActionGroup *toolbarActionGroup = new QActionGroup(this);

        QIcon splineIcon;
        splineIcon.addFile(QStringLiteral(":/images/spline.png"), QSize(), QIcon::Normal, QIcon::Off);
        brushSelectAction = new QAction(toolbarActionGroup);
        brushSelectAction->setCheckable(true);
        brushSelectAction->setChecked(false);
        brushSelectAction->setIcon(splineIcon);

        QIcon eraseIcon;
        eraseIcon.addFile(QStringLiteral(":/images/eraser.png"), QSize(), QIcon::Normal, QIcon::Off);
        eraseSelectAction = new QAction(toolbarActionGroup);
        eraseSelectAction->setCheckable(true);
        eraseSelectAction->setChecked(false);
        eraseSelectAction->setIcon(eraseIcon);

        QIcon marqueeIcon;
        marqueeIcon.addFile(QStringLiteral(":/images/marquee.png"), QSize(), QIcon::Normal, QIcon::Off);
        marqueeAction = new QAction(toolbarActionGroup);
        marqueeAction->setCheckable(true);
        marqueeAction->setChecked(false);
        marqueeAction->setIcon(marqueeIcon);

        QIcon playIcon;
        playIcon.addFile(QStringLiteral(":/images/play.png"), QSize(), QIcon::Normal,QIcon::Off);
        playAction = new QAction(toolbarActionGroup);
        playAction->setIcon(playIcon);
        playAction->setCheckable(true);
        playAction->setChecked(false);

        QIcon pauseIcon;
        pauseIcon.addFile(QStringLiteral(":/images/pause.png"), QSize(), QIcon::Normal,QIcon::Off);
        pauseAction = new QAction(toolbarActionGroup);
        pauseAction->setIcon(pauseIcon);
        pauseAction->setCheckable(true);

        QIcon resetIcon;
        resetIcon.addFile(QStringLiteral(":/images/reset.png"), QSize(), QIcon::Normal,QIcon::Off);
        resetAction = new QAction(toolbarActionGroup);
        resetAction->setIcon(resetIcon);
        pauseAction->setCheckable(true);

        searchAction = new QAction(this);
    }

    void UIManager::SetupToolbar()
    {
        mainToolbar = new QToolBar(parentWidget);
        mainToolbar->addAction(brushSelectAction);
        mainToolbar->addAction(eraseSelectAction);
        mainToolbar->addAction(marqueeAction);
        //---------------------------------
        mainToolbar->addAction(playAction);
        mainToolbar->addAction(pauseAction);
        mainToolbar->addAction(resetAction);

        //mainToolbar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        mainToolbar->setFloatable(true);
        mainToolbar->show();
    }

    void UIManager::SetUI()
    {
        CreateActions();
        SetupToolbar();
    }

}
