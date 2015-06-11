#pragma once
#include "abstractmodelitem.h"
#include "component.h"
#include "Box2D/Box2D.h"

namespace CDI
{
	class PhysicsManager;

	enum JointType {DISTANCE=10, FRICTION, GEAR,MOTOR,
					MOUSE, PRISMATIC, PULLEY, REVOLUTE,
					ROPE, WELD, WHEEL, NO_JOINT};
	/**
	 * @brief The PhysicsJoint class contains information
	 * related to joint objects used in simulation
	 */
	class PhysicsJoint : public AbstractModelItem
	{
	protected:
		JointType _jointType;
		b2Joint* joint;

		b2JointDef* jointDef;

	protected:
		PhysicsJoint(b2JointDef* jointDefinition, b2World* world);
		virtual ~PhysicsJoint();
		void createJointByType();
	public:
		inline ItemType type() { return ItemType::PHYSICSJOINT; }
		virtual JointType jointType();
		virtual b2JointDef* getJointDef();
//		virtual void setJointDef(b2JointDef* def);	// No setting allowed at present.
													// Check doc to see how to do so


	};

	//	class RevoluteJoint : public PhysicsJoint
	//	{
	//	public:
	//		b2JointDef* getJointDef() const;
	//		void setJointDef(b2JointDef* def);
	//	};
}
