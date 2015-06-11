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
			_device = QTabletEvent::NoDevice;
		}

        bool event(QEvent *event) Q_DECL_OVERRIDE;

		QTabletEvent::TabletDevice tabletDevice();

    protected:
        QTabletEvent::TabletDevice _device;

    signals:
        void OnStylusProximity(QEvent* event);
    };
}
