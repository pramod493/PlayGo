#include "tabletapplication.h"

#include <QString>
#include <QDebug>
#include "QsLog.h"

namespace CDI
{
	bool TabletApplication::event(QEvent *event)
	{
		/*if (event->type() == QEvent::TabletEnterProximity ||
				event->type() == QEvent::TabletLeaveProximity) {
			_device = static_cast<QTabletEvent *>(event)->device();

			emit OnStylusProximity(event);
			return true;
		}*/
		return QApplication::event(event);
	}

	QTabletEvent::TabletDevice TabletApplication::tabletDevice()
	{
		return _device;
	}
}
