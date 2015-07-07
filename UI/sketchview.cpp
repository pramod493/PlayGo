#include "sketchview.h"
#include "QsLog.h"
#include <QDebug>
#include <QMessageBox>
#include <QMimeData>
#include <QFileInfo>
#include <QUrl>
namespace CDI
{
	SketchView::SketchView(Page *page, QWidget* parent)
        : QGraphicsView(parent)
    {
		_page = page;
		qDebug() << "SketchView created";

		{	// Gesture stuff
			setAttribute(Qt::WA_AcceptTouchEvents, true);
			viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
//			grabGesture(Qt::PinchGesture);
//			viewport()->grabGesture(Qt::PinchGesture);
		}

		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        QLinearGradient gradient = QLinearGradient(0,0,0,1000);
        gradient.setColorAt(0.0,Qt::white);
		gradient.setColorAt(1.0, QColor(255,200,200,255));

		setBackgroundBrush(QBrush(gradient));

		_scene = new SketchScene(_page, this);
		setScene(_scene);
		setSceneRect(0,0,5000,5000);
		setRenderHint(QPainter::Antialiasing);
		setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

		// Rotate the view so that y-axis is positive in upwards direction
		QTransform t = QTransform();
//		t = t.rotate(180);
		t = t.translate(2500,2500);
		setTransform(t);

		setAcceptDrops(true);
    }

	SketchView::~SketchView()
    {
		// Need not delete the scene since its destructor will automatically
		// be called because its parent QObject is being destroyed here
    }

	void SketchView::drawBackground(QPainter * painter, const QRectF & rect)
	{
		QPen pen = QPen();
		pen.setWidth(1);
		pen.setColor(QColor(200,200,255)); pen.setStyle(Qt::DashLine);
		painter->setPen(pen);

		QRect int_rect = rect.toRect();

		int x_left = int_rect.left();	int y_min = int_rect.top();
		int x_right = int_rect.right();	int y_max= int_rect.bottom();

		x_left = (x_left/100) * 100;
		y_min = (y_min/100) * 100;

		int y_tmp = y_min;
		while (y_tmp < y_max)
		{
			painter->drawLine(x_left, y_tmp, x_right, y_tmp);
			y_tmp += 100;
		}

		while (x_left < x_right)
		{
			painter->drawLine(QPointF(x_left, y_min), QPointF(x_left, y_max));
			x_left += 100;
		}

		emit viewDrawbackground(painter, rect);
	}

	void SketchView::drawForeground(QPainter *painter, const QRectF &rect)
	{
		QGraphicsView::drawForeground(painter, rect);
		emit viewDrawforeground(painter, rect);
	}

	SketchScene* SketchView::getSketchScene()
	{
		return _scene;
	}

    // Handle the resizing of the main window
    void SketchView::resizeEvent(QResizeEvent *event)
    {
        if (scene() != NULL)
        {
            scene()->setSceneRect(QRect(QPoint(), event->size()));
        }
        QGraphicsView::resizeEvent(event);
    }

    bool SketchView::event(QEvent *event)
    {
		// We are grabbing gesture at viewport and therefore no need to capture events here
        switch (event->type())
        {
            case QEvent::TouchEnd :
            case QEvent::TouchUpdate :
            case QEvent::TouchBegin :
            {
            }
        }
        return QGraphicsView::event(event);
    }

    void SketchView::tabletEvent(QTabletEvent *event)
    {
        event->accept();
        switch (event->type())
        {
            case QEvent::TabletPress :
            case QEvent::TabletMove :
            case QEvent::TabletRelease :
            {
				emit viewTabletEvent(event, this);
                break;
            }
        }
    }

	void SketchView::wheelEvent(QWheelEvent* event)
	{
		event->setAccepted(true);
	}

	bool SketchView::viewportEvent(QEvent *event)
	{
		// Send the events to children first and then check is accpeted
		bool retval = QGraphicsView::viewportEvent(event);
		switch (event->type())
		{
		case QEvent::TouchBegin :
		case QEvent::TouchUpdate :
		case QEvent::TouchEnd :
		case QEvent::TouchCancel :
			if (event->isAccepted() == false)
			{
				emit viewTouchEvent(static_cast<QTouchEvent*>(event), this);
				event->accept();
			}
			break;
		case QEvent::Gesture :
//			Do not use gestures at the moment but they can be used later on
//			qDebug() << "Gesture received";
//			return true;
			break;
		}
		return retval;
	}


	void SketchView::dragEnterEvent(QDragEnterEvent *event)
	{
		foreach(QUrl url, event->mimeData()->urls())
			if (QFileInfo(url.toLocalFile()).suffix().toUpper()=="PNG")
			{
				event->acceptProposedAction();
				return;
			}
	}

	void SketchView::dragMoveEvent(QDragMoveEvent *event)
	{
		event->accept();
	}

	void SketchView::dragLeaveEvent(QDragLeaveEvent *event)
	{
		event->accept();
	}

	void SketchView::dropEvent(QDropEvent *event)
	{
		foreach(QUrl url, event->mimeData()->urls())
				{
					QString filename = url.toLocalFile();
					QString suffix = QFileInfo(filename).suffix().toUpper();
					if(suffix=="PNG")
					{
						event->acceptProposedAction();
						emit viewImageDrop(filename);
					}
				}
	}

}
