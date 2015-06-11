#include "physicsbody.h"
#include "polygon2d.h"
#include "component.h"

namespace CDI
{
	PhysicsBody::PhysicsBody(Component *parent, b2Body *boxBody)
	{
		shapes = QVector<Polygon2D*>();
		body = boxBody;
		component = parent;

	}

	PhysicsBody::~PhysicsBody()
	{

	}

	void PhysicsBody::setTransform(QTransform& transform)
	{
		if (component!= NULL)
			component->setTransform(transform);
	}

	QTransform PhysicsBody::transform() const
	{
		if (component!= NULL)
			return component->transform();
		return QTransform();
	}

	QTransform PhysicsBody::inverseTransform() const
	{
		if (component!= NULL) return component->inverseTransform();
		return QTransform();
	}
}
