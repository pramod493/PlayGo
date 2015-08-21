#pragma once
#include "abstractmodelitem.h"
#include "component.h"
#include "Box2D/Box2D.h"
#include <QGraphicsPathItem>

namespace CDI
{
	class PhysicsManager;
	class JointGraphics;
	/**
	 * @brief The cdJointDef class defines the abstract class for defining
	 */
	class cdJoint
	{
	private:
		b2JointType _boxJointType;

	public:
		cdJoint(b2JointType type) : _boxJointType(type) {}

		/**
		 * @brief jointType returns the joint type and cannot be changed after creation
		 * @return
		 * @remarks use for dynamic casting
		 */
		b2JointType jointType() const { return _boxJointType;}

		/**
		 * @brief joint returns the box2d joint pointer. Derived classes must implement
		 * @return box2d joint
		 */
		virtual b2Joint* joint() const = 0;

		/**
		 * @brief jointDef returns the joint params (Pure virtual)
		 * @return b2JointDef pointer
		 */
		virtual b2JointDef* jointDef() const = 0;
	};

	class cdPinJoint : public cdJoint
	{
	protected:
		PhysicsManager*			_physicsmanager;
		b2RevoluteJointDef*		_jointDef;
		b2RevoluteJoint*		_joint;
		Component*				_componentA;
		Component*				_componentB;
		Point2D					_relPosA;
		Point2D					_relPosB;

	public:
		cdPinJoint();

		virtual ~cdPinJoint();

		b2RevoluteJoint* joint() const override;

		b2RevoluteJointDef* jointDef() const override;

		Component *componentA() const;
		Component *componentB() const;
		Point2D relPosA() const;
		Point2D relPosB() const;

		friend class PhysicsManager;
	};

	class cdSliderJoint : public cdJoint
	{
	protected:
		PhysicsManager*			_physicsmanager;
		b2PrismaticJointDef*	_jointDef;
		b2PrismaticJoint*		_joint;
		Component*				_componentA;
		Component*				_componentB;
		Point2D					_relPosA;
		Point2D					_relPosB;

	public:
		cdSliderJoint();

		virtual ~cdSliderJoint();

		b2PrismaticJoint* joint() const override;

		b2PrismaticJointDef* jointDef() const override;

		Component *componentA() const;
		Component *componentB() const;
		Point2D relPosA() const;
		Point2D relPosB() const;

		friend class PhysicsManager;
	};

	class cdWeldJoint : public cdJoint
	{
	protected:
		PhysicsManager*		_physicsmanager;
		b2WeldJointDef*		_jointDef;
		b2WeldJoint*		_joint;
		Component*			_componentA;
		Component*			_componentB;
		Point2D				_relPosA;
		Point2D				_relPosB;

	public:
		cdWeldJoint();

		virtual ~cdWeldJoint();

		b2WeldJoint* joint() const override;

		b2WeldJointDef* jointDef() const override;

		Component *componentA() const;
		Component *componentB() const;
		Point2D relPosA() const;
		Point2D relPosB() const;

		friend class PhysicsManager;
	};

	class cdGearJoint : public cdJoint
	{
	protected:
		PhysicsManager*		_physicsmanager;
		b2GearJointDef*		_jointDef;
		b2GearJoint*		_joint;
		cdJoint*			_parentJointA;
		cdJoint*			_parentJointB;
	};

	/**
	 * @brief The PhysicsJoint class contains information
	 * related to joint objects used in simulation
	 * @remarks use b2JointType as enum to represent joints
	 */
//	class PhysicsJoint : public Item
//	{
//	protected:
//		PhysicsManager* _physicsManager;
//		b2JointType		_box2dJointType;
//		b2Joint*		_joint;
//		b2JointDef*		_jointDef;
//		QUuid			_jointID;
//		Component*		componentA;
//		Component*		componentB;

//		Point2D relPosA;
//		Point2D relPosB;

//	protected:
//		PhysicsJoint(PhysicsManager* physicsmanager = NULL);

//		virtual ~PhysicsJoint();

//		void createJointByType();
//	public:
//		ItemType type() const { return PHYSICSJOINT; }

//		QUuid id() const;

//		QDataStream& serialize(QDataStream &stream) const;

//		QDataStream& deserialize(QDataStream &stream);

//		virtual PhysicsManager* physicsManager() const { return _physicsManager; }

//		virtual b2Joint* joint();

//		virtual b2JointType jointType();

//		// NOTE - Might not be required
//		virtual b2JointDef* jointDef();

//		Component* getComponentA() const;

//		Component* getComponentB() const;

//		virtual bool updateJoint();

//		//virtual void updateJoint();

//		friend class PhysicsManager;
//	};

	//	class RevoluteJoint : public PhysicsJoint
	//	{
	//	public:
	//		b2JointDef* getJointDef() const;
	//		void setJointDef(b2JointDef* def);
	//	};

	class PhysicsJoint :public QObject, public QGraphicsPathItem
	{
		Q_OBJECT

	public:
		enum {Type = UserType + JOINTVIEW };

	protected:
		QUuid			_id;
		cdJoint			*_cdJoint;

	public:
		PhysicsJoint(b2JointType jointType, QGraphicsItem *parent = nullptr);

		virtual ~PhysicsJoint() {}

		int type() const { return Type; }

		QUuid id() const { return _id; }

		/**
		 * @brief Returns the expanded bounding rect for joint in order to ease the selection
		 * @return
		 */
		QRectF boundingRect() const;

		cdJoint* getcdjoint() const;

		virtual void initializePainterPath() = 0;

		friend class PhysicsManager;
	};

//	/**
//	 * @brief The PinJointGraphics class displays the pin joint between two
//	 * components. In case of new joint, it also displays the status of motor
//	 */
//	class PinJointGraphics : public JointGraphics
//	{
//	public :
//		PinJointGraphics(PhysicsJoint* physicsJoint, QGraphicsItem* parent = 0);
//		void initializePainterPath();
//	};

//	/**
//	 * @brief The SliderJointGraphics class dispalys the slider/prismatic joint
//	 * between two components
//	 */
//	class SliderJointGraphics : public JointGraphics
//	{
//	public:
//		SliderJointGraphics(PhysicsJoint* physicsJoint, QGraphicsItem* parent = 0)
//			: JointGraphics(physicsJoint, parent)
//		{}
//		void initializePainterPath() {}
//	};

}
