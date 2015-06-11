#pragma once

#include <QGraphicsPathItem>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>
#include "stroke.h"
#include "point2dpt.h"
#include "commonfunctions.h"
#include "cdi2qtwrapper.h"

namespace CDI
{
	class GraphicsPathItem : public QGraphicsPathItem
	{
	public:
		Stroke* parentStroke;

	public:
		// We can simpley replace Point2D with Point2DPT but not doing it now
		// Only allow to be created as children of a GraphicsItemGroup
		/**
		 * @brief GraphicsPathItem
		 * @param parent
		 * @param startPoint
		 * @param pressure
		 * @param time
		 */
		GraphicsPathItem(QGraphicsItem* parent, Point2D startPoint, float pressure=1.0f, int time = 0);

		GraphicsPathItem(QGraphicsItem* parent, Stroke* stroke);

		~GraphicsPathItem();

		int type() const { return QGraphicsItem::UserType + GraphicsItemType::STROKEVIEW; }

		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

		void push_back(QPointF point, float pressure=1.0, int time = 0);

		void push_back(Point2DPT point);

		void ApplySmoothing(int order);

		bool Selected(QPointF point, float extraWidth);

		QRectF boundingRect() const;

		Q_DECL_DEPRECATED void OnStrokeUpdate(Stroke* stroke);
	};
}
