#include "sketchview.h"
#include <QDebug>

namespace CDI
{
	SketchView::SketchView(Page *page, QWidget* parent)
        : QGraphicsView(parent)
    {
		_page = page;
        setAttribute(Qt::WA_AcceptTouchEvents, true);
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
		t = t.rotate(180);
		setTransform(t);
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
        switch (event->type())
        {
            case QEvent::TouchEnd :
            case QEvent::TouchUpdate :
            case QEvent::TouchBegin :
            {
                QTouchEvent* touchEvent = static_cast<QTouchEvent*>(event);
				emit viewTouchEvent(touchEvent, this);
				event->setAccepted(true);
				return true;
				/*
				qDebug() << "Touch:Pre:Points" << touchEvent->touchPoints().count() << "Pre-send";
				bool eventStatus = QGraphicsView::event(event);
				bool isAccepted = event->isAccepted();
				event->setAccepted(false);
				qDebug()  << "Touch:Pre:Points" << touchEvent->touchPoints().count() <<
							 "return:" << eventStatus << "accept:" << isAccepted;
				return false;

				if (touchEvent->touchPoints().count() == 2 && scene() != NULL)
				{
					const QTouchEvent::TouchPoint &touchPoint1 = touchEvent->touchPoints().first();
					const QTouchEvent::TouchPoint &touchPoint2 = touchEvent->touchPoints().last();
					QLineF line1(touchPoint1.lastScenePos(), touchPoint2.lastScenePos());
					QLineF line2(touchPoint1.scenePos(), touchPoint2.scenePos());
					qreal p_scale = line2.length()/line1.length();
					scale(p_scale,p_scale);
					qDebug() << "View scaling" << p_scale;
					// We did it. not sure how it's going to go down
				}
				return true;
				break;
				*/
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

}
