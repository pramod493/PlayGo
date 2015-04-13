#include "cdiwindow.h"

namespace CDI
{
    CDIWindow::CDIWindow(QWidget *parent) : QMainWindow(parent)
    {
        uimanager = new UIManager(this);

    }

    CDIWindow::~CDIWindow()
    {
		delete uimanager;
    }

	void CDIWindow::InitWidgets()
	{
		// Set up the main window propoerties
		setObjectName(QStringLiteral("Main Sketch Window"));
		setWindowTitle("CDI Sketcher - PlayGo");

		QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
		sizePolicy.setHorizontalStretch(0);
		setSizePolicy(sizePolicy);

		uimanager->SetUI();
	}
}
