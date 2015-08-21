#ifndef FORCEITEM_H
#define FORCEITEM_H
#include "commonfunctions.h"
#include "abstractmodelitem.h"
#include <QGraphicsLineItem>
#include <QGraphicsPolygonItem>
#include <QPen>
#include <QBrush>
#include <QLineF>
#include <QPainter>
#include <QVector2D>
#include "cdi2qtwrapper.h"

namespace CDI
{
	/*-------------------------------------------------------
	 *  ForceGraphicsItem class declaration
	 * ----------------------------------------------------*/
	class ForceGraphicsItem : public QGraphicsPolygonItem
	{
	public:
		static float forceScale;
		enum {Type = UserType + FORCEVIEW};
	protected:
		QUuid		_id;

		QPointF		_forcePosition;
		QPointF		_forceVector;

		/*             <---> 10 px
		*			   |\
		*	-----------| \  height = 14 px
		*	|6 px		  \ arrowhead = 10 px
		*	|			  /
		*	-----------| /
		*	l_vec	   |/
		*/
		int thickness_main;	// Why not hard code?
		int thickness_head;
		int length_head;

	public:
		ForceGraphicsItem(QGraphicsItem* parent = 0);

		ForceGraphicsItem(QPointF forceposition, QPointF forcevector, QGraphicsItem* parent = 0);

		virtual ~ForceGraphicsItem() {}

		/**
		 * @brief type returns the item type
		 * @return UserType + FORCEVIEW
		 */
		int type() const { return Type; }

		/**
		 * @brief setLine sets the force vector directly
		 * @param x0 Origin.x()
		 * @param y0 origin.y()
		 * @param fx Force.x()
		 * @param fy Force.y()
		 */
		void setLine(float x0, float y0, float fx, float fy);

		/**
		 * @brief forcePosition return the force position on the component
		 * @return Point of application of the force
		 * @remarks apply scaling before using on box
		 */
		QPointF forcePosition() const;

		/**
		 * @brief setForcePosition sets the point of application of force
		 * vector on the component
		 * @param pos Position of the force vector
		 */
		void setForcePosition(QPointF pos);

		/**
		 * @brief forceVector returns the force vector
		 * @return Force vector (2D)
		 */
		QPointF forceVector() const;

		/**
		 * @brief setForceVector
		 * @param pos
		 */
		void setForceVector(QPointF pos);

		/**
		 * @brief initialize sets up the force vector based on the current state
		 */
		virtual void initialize();

		QDataStream& serialize(QDataStream &stream) const;
		QDataStream& deserialize(QDataStream &stream);
	};

}
#endif // FORCEITEM_H
