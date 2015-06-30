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
	class GraphicsItemGroup;
	class GraphicsPathItem : public QGraphicsItem //QGraphicsPathItem
	{
	public:
		enum { Type = UserType + STROKEVIEW };

	protected:
		Stroke* _parentStroke;
		QPen _pen;
		QBrush _brush;

        bool _highlighted;

	public:

		/**
		 * @brief Create a new GraphicsPathItem object based on Stroke object
		 * @param parent parent QGraphicsItem object
		 * @param stroke Stroke object
		 */
		GraphicsPathItem(GraphicsItemGroup* parent, Stroke* stroke);

		~GraphicsPathItem();

		int type() const { return Type; }

		QBrush brush() const;

		QPen pen() const;

		Stroke* parentStroke() const;

		void setBrush(const QBrush &brush);

		void setPen(const QPen &pen);

		void setParentStroke(Stroke* stroke);

		void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

		bool contains(QPointF point);

		QRectF boundingRect() const;

		void push_back(QPointF point, float pressure=1.0, int time = 0);

		void push_back(Point2DPT point);

		void ApplySmoothing(int order);

		bool Selected(QPointF point, float extraWidth);

		void updateStroke();

        bool isHighlighted() const;

        void highlight(bool value);
	};
}
