#pragma once
#include <QApplication>
#include <QTabletEvent>
#include <QEvent>

namespace CDI
{
	/**
	 * @brief The TabletApplication class detects when a stylus is nearby and triggers
	 * OnStylusProximity event for those cases
	 */
    class TabletApplication : public QApplication
    {
        Q_OBJECT
    public:
        TabletApplication(int &argv, char **args)
			: QApplication(argv, args)
		{
			setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, false);
			setAttribute(Qt::AA_NativeWindows, false);
		}

        bool event(QEvent *event) Q_DECL_OVERRIDE;

    signals:
		void signalStylusEnter();
		void signalStylusLeave();
    };
}
