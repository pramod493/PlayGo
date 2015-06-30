#include "physicsjoint.h"
#include "QsLog.h"

namespace CDI
{
	// Some of the functions which need to be used locally.
	// Do not add these as class member because we want to
	// reduce dependency on physics engine in this part
	JointType getTypeByJointDef(b2JointDef* def)
	{
		switch (def->type)
		{
        case e_revoluteJoint :
            return REVOLUTE;
		case e_prismaticJoint :
            return PRISMATIC;
        case e_distanceJoint :
            return DISTANCE;
        case e_pulleyJoint :
            return PULLEY;
        case e_mouseJoint :
            return MOUSE;
        case e_gearJoint :
            return GEAR;
        case e_wheelJoint :
            return WHEEL;
        case e_weldJoint :
            return WELD;
        case e_frictionJoint :
            return FRICTION;
        case e_ropeJoint :
            return ROPE;
        case e_motorJoint :
            return MOTOR;
		}
        return NO_JOINT;
	}

	PhysicsJoint::PhysicsJoint(b2JointDef* jointDefinition, b2World* world)
	{
//		_jointType = CDI::getTypeByJointDef(jointDefinition);
	}

	PhysicsJoint::~PhysicsJoint()
	{

	}

	void PhysicsJoint::createJointByType()
	{
		if (_joint != NULL) QLOG_INFO() << "NO mechanism exists to delete joint"
									<< "@PhysicsJoint::createJointByType()";

//		switch (_jointType)
//		{
//		case DISTANCE :

//			break;
//		case FRICTION :
//			break;
//		case GEAR :
//			break;
//		case MOTOR :
//			break;
//		case MOUSE :
//			break;
//		case PRISMATIC :
//			break;
//		case PULLEY :
//			break;
//		case REVOLUTE :
//			break;
//		case ROPE :
//			break;
//		case WELD :
//			break;
//		case WHEEL :
//			break;

//		}
	}

	QUuid PhysicsJoint::id() const
	{
		return _jointID;
	}

	// TODO
	QDataStream& PhysicsJoint::serialize(QDataStream &stream) const
	{
		return stream;
	}

	// TODO
	QDataStream& PhysicsJoint::deserialize(QDataStream &stream)
	{
		return stream;
	}

	b2Joint* PhysicsJoint::joint()
	{
		return _joint;
	}

	b2JointType PhysicsJoint::jointType()
	{
		return _box2dJointType;
	}

	b2JointDef* PhysicsJoint::jointDef()
	{
		return _jointDef;
	}

}
