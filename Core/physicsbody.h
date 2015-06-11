#pragma once
#include "abstractmodelitem.h"
#include <vector>
#include <QVector>
#include <Box2D/Dynamics/b2Body.h>
namespace CDI
{
	class PhysicsManager;
	class Component;
	class Polygon2D;
	class PhysicsBody : public AbstractModelItem
	{
		Q_OBJECT
	public :
		b2Body* body;

		float density;	/**< Density of the material. Mass is calculate by Box2D */
		float friction;
		bool isStatic;

		/**
		 * @brief shapes stores the polygon which constitute
		 * the collision boundary of the object. This need to be attached to the object
		 */
		QVector<Polygon2D*> shapes;

	protected:
		Component* component;

	protected:
		PhysicsBody(Component* parent, b2Body* boxBody);

		virtual ~PhysicsBody();

		b2Body* getBox2DBody() { return body;}

	public:
		ItemType type() const { return ItemType::PHYSICSBODY; }

		void setTransform(QTransform& transform);

		QTransform transform() const;

		QTransform inverseTransform() const;

		void updateParentTransform();

		QDataStream& serialize(QDataStream& stream) const { return stream; }

		QDataStream& deserialize(QDataStream& stream) { return stream; }

		friend class PhysicsManager;
	};
}
