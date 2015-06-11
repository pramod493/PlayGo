#include "physicsjoint.h"
#include <QDebug>

namespace CDI
{
	// Some of the functions which need to be used locally.
	// Do not add these as class member because we want to
	// reduce dependency on physics engine in this part
	JointType getTypeByJointDef(b2JointDef* def)
	{
		switch (def->type)
		{
		case b2JointType::e_revoluteJoint :
			return JointType::REVOLUTE;
		case e_prismaticJoint :
			return JointType::PRISMATIC;
		case b2JointType::e_distanceJoint :
			return JointType::DISTANCE;
		case b2JointType::e_pulleyJoint :
			return JointType::PULLEY;
		case b2JointType::e_mouseJoint :
			return JointType::MOUSE;
		case b2JointType::e_gearJoint :
			return JointType::GEAR;
		case b2JointType::e_wheelJoint :
			return JointType::WHEEL;
		case b2JointType::e_weldJoint :
			return JointType::WELD;
		case b2JointType::e_frictionJoint :
			return JointType::FRICTION;
		case b2JointType::e_ropeJoint :
			return JointType::ROPE;
		case b2JointType::e_motorJoint :
			return JointType::MOTOR;
		}
		return JointType::NO_JOINT;
	}

	PhysicsJoint::PhysicsJoint(b2JointDef* jointDefinition, b2World* world)
	{
		_jointType = CDI::getTypeByJointDef(jointDefinition);
	}

	PhysicsJoint::~PhysicsJoint()
	{

	}

	void PhysicsJoint::createJointByType()
	{
		if (joint != NULL) qDebug() << "NO mechanism exists to delete joint"
									<< "@PhysicsJoint::createJointByType()";

		switch (_jointType)
		{
		case DISTANCE :

			break;
		case FRICTION :
			break;
		case GEAR :
			break;
		case MOTOR :
			break;
		case MOUSE :
			break;
		case PRISMATIC :
			break;
		case PULLEY :
			break;
		case REVOLUTE :
			break;
		case ROPE :
			break;
		case WELD :
			break;
		case WHEEL :
			break;

		}
	}

	JointType PhysicsJoint::jointType()
	{
		return _jointType;
	}

	b2JointDef* PhysicsJoint::getJointDef()
	{
		return jointDef;
	}

}
