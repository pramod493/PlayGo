#include <QtWidgets>
#include "tabletapplication.h"
#include <QMessageBox>
#include <QString>
#include <QEvent>
#include <QString>
#include <iostream>

namespace CDI
{
    bool TabletApplication::event(QEvent *event)
    {
        if (event->type() == QEvent::TabletEnterProximity ||
                event->type() == QEvent::TabletLeaveProximity) {

            _device = static_cast<QTabletEvent *>(event)->device();
            emit OnStylusProximity(event);
            return true;
        }
        return QApplication::event(event);
    }
}
