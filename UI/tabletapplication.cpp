#include "tabletapplication.h"

#include <QString>
#include <QDebug>
#include "QsLog.h"

namespace CDI
{
	bool TabletApplication::event(QEvent *event)
	{
		if (event->type() == QEvent::TabletEnterProximity )
		{
			emit signalStylusEnter();
		}
		else if (event->type() == QEvent::TabletLeaveProximity)
		{
			emit signalStylusLeave();
		}
		return QApplication::event(event);
	}
}
