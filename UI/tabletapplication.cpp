#include "tabletapplication.h"

#include <QString>
#include <QDebug>

namespace CDI
{
	bool TabletApplication::event(QEvent *event)
	{
		if (event->type() == QEvent::TabletEnterProximity ||
				event->type() == QEvent::TabletLeaveProximity) {
			_device = static_cast<QTabletEvent *>(event)->device();
			{
				if (event->type() == QEvent::TabletEnterProximity)
				qDebug() << "Enter proximity with device" << _device;
				else if (event->type() == QEvent::TabletLeaveProximity)
					qDebug() << "Leave proximity with device" << _device;
			}
			emit OnStylusProximity(event);
			return true;
		}
		return QApplication::event(event);
	}

	QTabletEvent::TabletDevice TabletApplication::tabletDevice()
	{
		return _device;
	}

}
