#pragma once
#include "abstractmodelitem.h"
#include "component.h"
#include "Box2D/Box2D.h"

namespace CDI
{
	class PhysicsManager;

	class PhysicsBody;

	/**
	 * @brief The PhysicsJoint class contains information
	 * related to joint objects used in simulation
	 * @remarks use b2JointType as enum to represent joints
	 */
	class PhysicsJoint : public Item
	{
	protected:
		PhysicsManager* _physicsManager;
		b2JointType		_box2dJointType;
		b2Joint*		_joint;
		b2JointDef*		_jointDef;
		QUuid			_jointID;
		Component*		componentA;
		Component*		componentB;

		Point2D relPosA;
		Point2D relPosB;

	protected:
		PhysicsJoint(PhysicsManager* physicsmanager = NULL);

		virtual ~PhysicsJoint();

		void createJointByType();
	public:
		ItemType type() const { return PHYSICSJOINT; }

		QUuid id() const;

		QDataStream& serialize(QDataStream &stream) const;

		QDataStream& deserialize(QDataStream &stream);

		virtual PhysicsManager* physicsManager() const { return _physicsManager; }

		virtual b2Joint* joint();

		virtual b2JointType jointType();

		// NOTE - Might not be required
		virtual b2JointDef* jointDef();

		//virtual void updateJoint();

		friend class PhysicsManager;

	};

	//	class RevoluteJoint : public PhysicsJoint
	//	{
	//	public:
	//		b2JointDef* getJointDef() const;
	//		void setJointDef(b2JointDef* def);
	//	};
}
