#include "physicsbody.h"
#include "polygon2d.h"
#include "component.h"

namespace CDI
{
	PhysicsBody::PhysicsBody(Component *component, b2Body *boxBody)
	{
		setParentItem(component);
		shapes = QVector<Polygon2D*>();
		_b2body = boxBody;

	}

	PhysicsBody::~PhysicsBody()
	{
		if (parentItem()!= NULL) parentItem()->removeItem(this);
	}

	b2Body* PhysicsBody::getBox2DBody()
	{
		return _b2body;
	}

	ItemType PhysicsBody::type() const
	{
		return ItemType::PHYSICSBODY;
	}

	QTransform PhysicsBody::transform() const
	{
		if (parentItem() != NULL)
			return parentItem()->transform();
		return QTransform();
	}

	void PhysicsBody::setTransform(QTransform transform)
	{
		if (component!= NULL)
			component->setTransform(transform);
	}

	QTransform PhysicsBody::globalTransform() const
	{
		if (component != NULL)
			return component->globalTransform();
		return QTransform();
	}

	QTransform PhysicsBody::inverseTransform() const
	{
		if (component!= NULL) return component->transform().inverted();
		return QTransform();
	}

	QDebug operator <<(QDebug d, const PhysicsBody& item)
	{
		d.nospace() << "Physics Body\n";
		d.nospace() << "ID: " << item.id() << "\n";
		d.nospace() << "Contains " << item.triangles.size() << " triangles";

		return d;
	}
}
