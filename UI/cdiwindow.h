#pragma once
#include <QMainWindow>
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
#include <QSlider>
#include <QVBoxLayout>
#include <QTabletEvent>

#include "playgo.h"

#include "sketchview.h"
#include "sketchscene.h"

namespace CDI
{
	class CDIWindow : public QMainWindow
	{
		Q_OBJECT

	public:
        //// datastructure components
        PlayGo *playgo;

        //// Datastructure components end
		QSize windowSize;

		QString title;
		// TODO - Create action groups in order to simplify interactions as well as menus
		// actions

		// File operations
		QAction* newAction;
		QAction* openPageAction;
		QAction* closePageAction;
		QAction* saveImageAction;

		// Sketching related operations
		QAction* brushSelectAction;
		QAction* eraseSelectAction;
		QAction* marqueeAction;
        //QAction* selectorAction;        // Enable selection of components.
		QAction* searchAction;          // Search the marqueed region
        //QAction* marqueeEnableAction;   // Enable the marquee widget

		// Simulation managed actions
		QAction *playAction, *pauseAction, *resetAction;

		// Toolbars
		QToolBar* mainToolbar;

		// menus
		// TODO - Menus are not needed even though they are good to have
		QMenuBar* menubar;
		QMenu* editMenu;
		QMenu* helpmenu;

		QSlider* brushWidthSlider;

		QVBoxLayout* mainLayout;
		// Sketching related widgets
		SketchView* sketchView;          // Primarily use this one. Skip the other ones...
		SketchScene* sketchScene;

		QGraphicsView* searchView;
		QGraphicsView* gestureView;
		QGraphicsScene* searchScene;
		QGraphicsScene* gestureScene;

		// Status bar displaye hints and user actions
		QStatusBar* statusBar;

	protected:
		QTabletEvent::TabletDevice tabletDevice;

	public:
		explicit CDIWindow(QWidget *parent = 0);

		~CDIWindow();

		void InitWidgets();

	protected:
		void CreateActions();

		void SetupToolbar();

		void CreateStatusbar();

		void closeEvent(QCloseEvent *event);

	signals:

		void signalBrushSizeChanged(int size);

	public slots:
		void setBrushValue(int size);

		void slotOnSignalProximity(QEvent* event);

        void Save();

	};
}
