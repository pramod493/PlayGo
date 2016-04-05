#pragma once
#include <QMainWindow>
#include <QSplitter>
#include <QColorDialog>
#include <QDir>
#include <QFile>
#include <QObject>
#include <QAction>
#include <QActionGroup>
#include <QToolBar>
#include <QIcon>
#include <QStatusBar>
#include <QSlider>
#include <QStackedLayout>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QTabletEvent>
#include "colorselectortoolbar.h"

#include "sketchview.h"
#include "sketchscene.h"
#include "PlayGoController.h"
#include "cdi2qtwrapper.h"

class QDockWidget;

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
		// TODO - Create action groups in order to simplify interactions as well
		//as menus actions

		// File operations
		QAction *newAction,*openPageAction,*closePageAction,*saveImageAction,
				*closeAction, *cameraLoadAction;

		// Sketching related operations
		QAction *brushSelectAction, *polygonSelectAction, *eraseSelectAction, *marqueeAction,						*searchAction,*connectModeAction;

		//QAction* selectorAction;        // Enable selection of components.
		//QAction* marqueeEnableAction;   // Enable the marquee widget

		// Simulation managed actions
		QAction *playAction, *pauseAction, *resetAction;

		// group ungroup action
		QAction *groupAction, *ungroupAction;

		// Toolbars
		QToolBar *mainToolbar;
		ColorSelectorToolbar* colorToolbar;

		// MENUS
		// TODO - Menus are not needed even though they are good to have
		QMenuBar *menubar, *editMenu, *helpmenu;

		QSlider* brushWidthSlider;

		QSplitter *splitter;

		QDockWidget* searchDock;

		// Sketching related widgets
		SketchView* sketchView;			// Primarily use this one. Skip the
										// other ones...
		PlayGoController* controller;

	protected:
		QTabletEvent::TabletDevice tabletDevice;

	public:
		explicit CDIWindow(QWidget *parent = 0);

		~CDIWindow();

		void initWidgets();

		void loadPage(Page* page);

	protected:
		void createActions();

		void setupToolbar();

		void createStatusbar();

		void connectActions();

		void closeEvent(QCloseEvent *event);

	signals:
		void onBrushSizeChanged(int size);

	public slots:
		/**
		 * @brief Clear removes all the items from the scene.
		 * \todo Also prompt user to save the current scene
		 */
		void clear();

		/**
		 * @brief save triggers saving of the current scene
		 * \todo Implement this feature
		 */
		void save();

		void exit();

		void setMode(UI::MODE newmode);

		void setBrushWidth(int size);

		void setBrushColor(QString, QColor);

		void onSearchTrigger();
		/**
		 * @brief onStylusEnter is called when a tablet device is near
		 * Use this to disable mouse input as well as other tasks
		 */
		void onStylusEnter();

		void onStylusLeave();

		void startSimulation();

		void pauseSimulation();

		void cameraLoad();
	};
}
