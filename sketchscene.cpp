#include "sketchscene.h"

namespace CDI
{
	SketchScene::SketchScene(QObject* parent) :
		QGraphicsScene(parent)
	{
		mouse_mode_enabled = true;	// Keep only for the testing stages.

		defaultPen = QPen(QColor(0,0,0));
		defaultPen.setStyle(Qt::SolidLine);
		defaultPen.setWidth(3.0);

		highlightPen = QPen(QColor(0.5,0,1.0));
		highlightPen.setStyle(Qt::SolidLine);
		highlightPen.setWidth(5.0);

		marqueeSelectPen = QPen(QColor(0.25,0.25,0.25));
		marqueeSelectPen.setStyle(Qt::DashLine);
		marqueeSelectPen.setWidth(2.5);

		defaultBrush = QBrush(QColor(1.0,1.0,1.0), Qt::NoBrush);
		fillBrush = QBrush(QColor(0.75,0.75,0.75), Qt::SolidPattern);

		searchResults = QList<CDISearchGraphicsItem*>();
		freeStrokes = QList<CDIGraphicsPathItem*>();

		current_mode = Draw;

		parent_item = NULL;

		drawing_right_now = false;

		current_stroke = NULL;

		setBackgroundBrush(fillBrush);
	}

	SketchScene::~SketchScene()
	{

	}

	void SketchScene::drawBackground(QPainter* painter, const QRectF &rect)
	{
		QGraphicsScene::drawBackground(painter, rect);
		//        addRect(2,3,200,150,defaultPen,fillBrush);
	}

	void SketchScene::clear()
	{
		// Delete all items and clear the scene
		size_t n = searchResults.size();
		for (int i = 0; i < n; i++)
		{
			CDISearchGraphicsItem* item = searchResults[i];
			delete item;
		}
		searchResults.clear();

		n = freeStrokes.size();
		for (int i=0; i<n; i++)
		{
			CDIGraphicsPathItem* item = freeStrokes[i];
			delete item;
		}
		freeStrokes.clear();

	}

	void SketchScene::BrushPress(QPointF scenePos)
	{
		if (0) {
			QGraphicsLineItem* line = new QGraphicsLineItem();
			line->setPen(marqueeSelectPen);
			line->setLine(QLineF(scenePos, QPointF(500,300)));
			addItem(line);
			//QGraphicsLineItem* line = addLine(QLineF(scenePos, QPointF(500,300)), defaultPen);
		}

		current_stroke = new CDIGraphicsPathItem(parent_item, scenePos);
		addItem(current_stroke);
		current_stroke->setPen(defaultPen);
	}

	void SketchScene::BrushMove(QPointF scenePos)
	{
		if (!(current_stroke == NULL))
			current_stroke->push_back(scenePos);
	}

	void SketchScene::BrushRelease(QPointF scenePos)
	{
		if (!(current_stroke == NULL))
			current_stroke->push_back(scenePos);

		// Trigger signal in order to update related/connected components
		current_stroke = NULL;
	}

	void SketchScene::SaveScene(QString file)
	{
		Q_UNUSED(file);
	}

	SketchScene* SketchScene::Clone()
	{
		return this;
	}

	// Protected function
	void SketchScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
	{
		mouseEvent->accept();
		if (!mouse_mode_enabled) return;

		switch (current_mode)
		{
		case MODE::Draw :
			BrushPress(mouseEvent->scenePos());
			break;
		case MODE::Erase :
			break;
		case MODE::Transform :
			break;
		case MODE::Edit :
			break;
		}
	}

	void SketchScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
	{
		mouseEvent->accept();
		if (!mouse_mode_enabled) return;

		switch (current_mode)
		{
		case MODE::Draw :
			BrushMove(mouseEvent->scenePos());
			break;
		case MODE::Erase :
			break;
		case MODE::Transform :
			break;
		case MODE::Edit :
			break;
		}
	}

	void SketchScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
	{
		mouseEvent->accept();
		if (!mouse_mode_enabled) return;

		switch (current_mode)
		{
		case MODE::Draw :
			BrushRelease(mouseEvent->scenePos());
			break;
		case MODE::Erase :
			break;
		case MODE::Transform :
			break;
		case MODE::Edit :
			break;
		}
	}


	void SketchScene::DrawAction(QTabletEvent *event, QPointF scenePos)
	{
		switch (event->type())
		{
		case QEvent::TabletPress :
			BrushPress(scenePos);
			break;
		case QEvent::TabletMove :
			BrushMove(scenePos);
			break;
		case QEvent::TabletRelease :
			BrushRelease(scenePos);
			break;
		}
	}

	void SketchScene::EraseAction(QTabletEvent *event, QPointF scenePos)
	{
		Q_UNUSED(scenePos);
		switch (event->type())
		{
		case QEvent::TabletPress :
			break;
		case QEvent::TabletMove :
			break;
		case QEvent::TabletRelease :
			break;
		}
	}

	void SketchScene::SelectAction(QTabletEvent *event, QPointF scenePos)
	{
		Q_UNUSED(scenePos);
		switch (event->type())
		{
		case QEvent::TabletPress :
			break;
		case QEvent::TabletMove :
			break;
		case QEvent::TabletRelease :
			break;
		}
	}

	//

	void SketchScene::slotTabletEvent(QTabletEvent* event, QPointF scenePos)
	{
		if (event->pointerType() == QTabletEvent::Pen)
			DrawAction(event, scenePos);
		if (event->pointerType() == QTabletEvent::Eraser)
			EraseAction(event, scenePos);
	}

	void SketchScene::slotSetSceneMode(MODE newMode)
	{
		current_mode = newMode;
		emit ModeChanged(newMode);
	}
}
