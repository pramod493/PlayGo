#include "sketchview.h"
#include "QsLog.h"
#include <QDebug>
#include <QMessageBox>
#include <QMimeData>
#include <QFileInfo>
#include <QUrl>

#include "QsLog.h"

namespace CDI
{
	SketchView::SketchView(Page *page, QWidget* parent)
		: QGraphicsView(parent)
	{
		_page = page;

		{	// Gesture stuff
			viewport()->setAttribute(Qt::WA_AcceptTouchEvents, true);
			viewport()->grabGesture(Qt::TapGesture, Qt::DontStartGestureOnChildren);
			viewport()->grabGesture(Qt::TapAndHoldGesture, Qt::DontStartGestureOnChildren);
			viewport()->grabGesture(Qt::PanGesture, Qt::DontStartGestureOnChildren);
			viewport()->grabGesture(Qt::SwipeGesture, Qt::DontStartGestureOnChildren);
		}

		// Keep the scrollbar hidden
		setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

		// Define the background gradient
		QLinearGradient gradient = QLinearGradient(0,0,0,1000);
		gradient.setColorAt(0.0,Qt::red);
		gradient.setColorAt(1.0, QColor(255,200,200,255));

		setBackgroundBrush(QBrush(gradient));

		// Create scene to contain _page
		QGraphicsScene *scene = page->scene();
		setScene(scene);
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
		// Nothing to do here
	}

	void SketchView::setPage(Page *page)
	{
		if (page)
		{
			_page = page;
			setScene(_page->scene());

			setSceneRect(0,0,5000,5000);
			setRenderHint(QPainter::Antialiasing);
			setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
		} else {
			_page = NULL;
			setScene(NULL);
		}
	}

	Page* SketchView::getPage() const
	{
		return _page;
	}

	void SketchView::drawBackground(QPainter * painter, const QRectF & rect)
	{
		// Call this to draw background brush as well
		//QGraphicsView::drawBackground(painter, rect);
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

	// Handle the resizing of the main window
	void SketchView::resizeEvent(QResizeEvent *event)
	{
		if (scene() != NULL)
		{
			scene()->setSceneRect(QRect(QPoint(), event->size()));
		}
		QGraphicsView::resizeEvent(event);
	}

	void SketchView::wheelEvent(QWheelEvent* event)
	{
		event->setAccepted(true);// Accept to ignore
	}

	void SketchView::dragEnterEvent(QDragEnterEvent *event)
	{
		if (event->mimeData()->hasHtml())
		{
			QLOG_INFO() << "Contains HTML";
		}
		if (event->mimeData()->hasImage())
		{
			QLOG_INFO() << "Contains Image";
		}
		if (event->mimeData()->hasText())
		{
			QLOG_INFO() << "Contains text";
		}
		if (event->mimeData()->hasUrls())
		{
			QLOG_INFO() << "Contains URLs";
		}

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
//					QString suffix = QFileInfo(filename).suffix().toUpper();

					event->acceptProposedAction();
					emit viewImageDrop(filename, this, event);
					// Check is done before itself
				}
	}

}
