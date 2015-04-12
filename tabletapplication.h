#pragma once

#include <QApplication>
#include <QTabletEvent>

namespace CDI
{
    class TabletApplication : public QApplication
    {
        Q_OBJECT

    public:
        TabletApplication(int &argv, char **args)
            : QApplication(argv, args) {

        }

        bool event(QEvent *event) Q_DECL_OVERRIDE;
    protected:
        QTabletEvent::TabletDevice _device;
    signals:
        void OnStylusProximity(QEvent* event);
    };
}
