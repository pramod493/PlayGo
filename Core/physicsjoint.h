#pragma once
#include "abstractmodelitem.h"
#include "component.h"
#include "Box2D/Box2D.h"

namespace CDI
{
	class PhysicsManager;

	class PhysicsBody;

	enum JointType {DISTANCE=10, FRICTION, GEAR,MOTOR,
					MOUSE, PRISMATIC, PULLEY, REVOLUTE,
					ROPE, WELD, WHEEL, NO_JOINT};
	/**
	 * @brief The PhysicsJoint class contains information
	 * related to joint objects used in simulation
	 * @remarks use b2JointType as enum to represent joints
	 */
	class PhysicsJoint : public Item
	{
	protected:
//		JointType _jointType;
		b2JointType		_box2dJointType;
		b2Joint*		_joint;
		b2JointDef*		_jointDef;
		QUuid			_jointID;

	public:
		PhysicsJoint(b2JointDef* jointDefinition, b2World* world);

		virtual ~PhysicsJoint();

		void createJointByType();
	public:
		virtual ItemType type() { return PHYSICSJOINT; }

		QUuid id() const;

		QDataStream& serialize(QDataStream &stream) const;

		QDataStream& deserialize(QDataStream &stream);

		virtual b2Joint* joint();

		virtual b2JointType jointType();

		virtual b2JointDef* jointDef();

	};

	//	class RevoluteJoint : public PhysicsJoint
	//	{
	//	public:
	//		b2JointDef* getJointDef() const;
	//		void setJointDef(b2JointDef* def);
	//	};
}
