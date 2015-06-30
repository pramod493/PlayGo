#pragma once
#include "abstractmodelitem.h"
#include <vector>
#include <QVector>
#include <Box2D/Dynamics/b2Body.h>
#include "poly2tri.h"
#include <QDebug>

namespace CDI
{
	class PhysicsManager;
	class Component;
	class Polygon2D;
	class PhysicsBody : public AbstractModelItem
	{
	protected:
		b2Body* _b2body;

		float density;	/**< Density of the material. Mass is calculate by Box2D */
		float friction;
		bool isStatic;

		/**
		 * @brief shapes stores the polygon which constitute
		 * the collision boundary of the object. This need to be attached to the object
		 */
		QVector<Polygon2D*> shapes;

		/**
		 * @brief CCW polygonized triangles
		 */
		std::vector<p2t::Triangle*> triangles;

		Component* component;

	public:
		//-----------------------------------------------
		// Constructors/Destructors
		//-----------------------------------------------
		PhysicsBody(Component* parent, b2Body* boxBody);

		virtual ~PhysicsBody();
		//-----------------------------------------------
		// Query/Set functions(same order in derived class)
		// Non-virtual
		//-----------------------------------------------
		b2Body* getBox2DBody();

		b2Vec2& getPosition();

		float getRotation();
		//-----------------------------------------------
		// Virtual functions (same order in derived class)
		//-----------------------------------------------
		ItemType type() const;// { return PHYSICSBODY; }

		QTransform transform() const;

		void setTransform(QTransform transform);

		QTransform globalTransform() const;

		QTransform inverseTransform() const;

		QDataStream& serialize(QDataStream& stream) const { return stream; }

		QDataStream& deserialize(QDataStream& stream) { return stream; }

		//-----------------------------------------------
		// Other functions not related to query/set
		//-----------------------------------------------
		void updateParentTransform();

		friend QDebug operator <<(QDebug d, const PhysicsBody& item);

		friend class PhysicsManager;
	};
}
