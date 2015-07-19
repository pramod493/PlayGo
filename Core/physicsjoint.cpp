#include "physicsjoint.h"
#include "QsLog.h"

namespace CDI
{
	/*
	// Some of the functions which need to be used locally.
	// Do not add these as class member because we want to
	// reduce dependency on physics engine in this part
	*/

	PhysicsJoint::PhysicsJoint(PhysicsManager* physicsmanager)
	{
		_physicsManager = physicsmanager;
		_box2dJointType = e_unknownJoint;

		_joint		= NULL;
		_jointDef	= NULL;

		_jointID = uniqueHash();

		componentA = NULL;
		componentB = NULL;

		relPosA = Point2D(0,0);
		relPosB = Point2D(0,0);
	}

	PhysicsJoint::~PhysicsJoint()
	{
		if (_joint)
		{
			// How to delete this one?
		}

		if (_jointDef) delete _jointDef;

	}

	void PhysicsJoint::createJointByType()
	{
		if (_joint != NULL)
			QLOG_INFO() << "No mechanism exists to delete joint"
						<< "@PhysicsJoint::createJointByType()";

	}

	QUuid PhysicsJoint::id() const
	{
		return _jointID;
	}

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
