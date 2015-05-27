#pragma once
#include <QGraphicsView>
#include <QTabletEvent>
#include <QBrush>
#include <QString>
#include <QGraphicsScene>

namespace CDI
{
	class SketchView : public QGraphicsView
	{
		Q_OBJECT
//	protected:
//		QGraphicsScene* scene;

	public:
		SketchView(QWidget* parent = 0);

		~SketchView();

		void SaveScene();

	protected:
		void resizeEvent(QResizeEvent *event);

		bool event(QEvent* event);

		void tabletEvent(QTabletEvent *event);

	signals:
		void signalViewTabletEvent(QTabletEvent *tabletEvent, QPointF scenePos);

	};


}
