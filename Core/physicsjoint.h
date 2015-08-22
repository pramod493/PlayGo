#pragma once
#include "component.h"
#include "Box2D/Box2D.h"
#include <QGraphicsPathItem>

namespace CDI
{
	class PhysicsManager;
	class PhysicsJoint;
	/**
	 * @brief The cdJointDef class defines the abstract class for defining
	 */
	class cdJoint : public QObject, public QGraphicsPathItem	// Abstract class for joint
	{
	public:
		enum {Type = UserType + JOINTVIEW};

	protected:
		bool _dirtyInternals;
		QUuid _id;

	private:
		b2JointType _boxJointType;

	public:
		cdJoint(b2JointType type, QGraphicsItem* parent)
			: QObject(), QGraphicsPathItem(parent),
			_boxJointType(type), _id(uniqueHash()) {}

		/**
		 * @brief jointType returns the joint type and cannot be changed after creation
		 * @return
		 * @remarks use for dynamic casting
		 */
		b2JointType jointType() const { return _boxJointType;}

		/**
		 * @brief type Returns the Item type for propert casting
		 * @return
		 */
		int type() const { return Type; }

		/**
		 * @brief id returns the item id
		 * @return Item ID
		 */
		QUuid id() const { return _id; }

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

		/**
		 * @brief isDirty returns true if the joint requires updating
		 * @return
		 */
		virtual bool isDirty() { return false; }

		/**
		 * @brief setDirty sets the joint as dirty. Recreate joint params based
		 * on joint definition after update is called
		 */
		virtual void setDirty() { _dirtyInternals = true; }

		/**
		 * @brief updateJoint calls appropriate functions to update the joint
		 * based on updated b2JointDef parameters
		 * @remarks Implementtion optional
		 */
		virtual void updateJoint() = 0;
	};

	class cdPinJoint : public cdJoint
	{
		Q_OBJECT
		Q_PROPERTY(bool motorEnabled READ isMotorEnabled)
		Q_PROPERTY(float maxMotorTorque READ maxMotorTorque)
		Q_PROPERTY(float motorSpeed READ motorSpeed)

		Q_PROPERTY(bool limitsEnabled READ isLimitsEnabled())
		Q_PROPERTY(float lowerLimit READ lowerLimit)
		Q_PROPERTY(float upperLimit READ upperLimit)

	protected:
		PhysicsManager*			_physicsmanager;
		b2RevoluteJointDef*		_jointDef;
		b2RevoluteJoint*		_joint;
		Component*				_componentA;
		Component*				_componentB;
		Point2D					_relPosA;
		Point2D					_relPosB;

	public:
		cdPinJoint(QGraphicsItem* parent = nullptr);
		virtual ~cdPinJoint();

		b2RevoluteJoint* joint() const override;
		b2RevoluteJointDef* jointDef() const override;
		// update joint
		void updateJoint();

		// Query functions
		Component *componentA() const;
		Component *componentB() const;
		Point2D relPosA() const;
		Point2D relPosB() const;

		// returns the current state of motor torque
		bool isMotorEnabled() const;
		float maxMotorTorque() const;
		float motorSpeed() const;

		float isLimitsEnabled() const;
		float lowerLimit() const;
		float upperLimit() const;

		float referenceAngle() const;
		float jointAngle() const;

	signals:
		void onJointChange(cdPinJoint*);

		friend class PhysicsManager;
	};

//	class cdSliderJoint : public cdJoint
//	{
//	protected:
//		PhysicsManager*			_physicsmanager;
//		b2PrismaticJointDef*	_jointDef;
//		b2PrismaticJoint*		_joint;
//		Component*				_componentA;
//		Component*				_componentB;
//		Point2D					_relPosA;
//		Point2D					_relPosB;

//	public:
//		cdSliderJoint();

//		virtual ~cdSliderJoint();

//		b2PrismaticJoint* joint() const override;

//		b2PrismaticJointDef* jointDef() const override;

//		Component *componentA() const;
//		Component *componentB() const;
//		Point2D relPosA() const;
//		Point2D relPosB() const;

//		friend class PhysicsManager;
//	};

//	class cdWeldJoint : public cdJoint
//	{
//	protected:
//		PhysicsManager*		_physicsmanager;
//		b2WeldJointDef*		_jointDef;
//		b2WeldJoint*		_joint;
//		Component*			_componentA;
//		Component*			_componentB;
//		Point2D				_relPosA;
//		Point2D				_relPosB;

//	public:
//		cdWeldJoint();

//		virtual ~cdWeldJoint();

//		b2WeldJoint* joint() const override;

//		b2WeldJointDef* jointDef() const override;

//		Component *componentA() const;
//		Component *componentB() const;
//		Point2D relPosA() const;
//		Point2D relPosB() const;

//		friend class PhysicsManager;
//	};

//	class cdGearJoint : public cdJoint
//	{
//	protected:
//		PhysicsManager*		_physicsmanager;
//		b2GearJointDef*		_jointDef;
//		b2GearJoint*		_joint;
//		cdJoint*			_parentJointA;
//		cdJoint*			_parentJointB;
//	};

//	class PhysicsJoint :public QObject, public QGraphicsPathItem
//	{
//		Q_OBJECT
//	public:
//		enum {Type = UserType + JOINTVIEW };

//	protected:
//		QUuid			_id;
//		cdJoint			*_cdJoint;

//	public:
//		PhysicsJoint(b2JointType jointType, QGraphicsItem *parent = nullptr);

//		virtual ~PhysicsJoint();

//		int type() const { return Type; }

//		QUuid id() const;

//		bool updateJoint();

//		/**
//		 * @brief Returns the expanded bounding rect for joint in order to ease the selection
//		 * @return
//		 */
//		QRectF boundingRect() const;

//		virtual cdJoint* getcdjoint() const;

//		virtual void initializePainterPath();

//		friend class PhysicsManager;
//	};
}
