#include "physicsjoint.h"
#include "physicsmanager.h"
#include "QsLog.h"
#include <QRadialGradient>
#include <QFont>
#include <QString>

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
		if (_jointDef) delete _jointDef;
		if (componentA) componentA->removeJoint(this);
		if (componentB) componentB->removeJoint(this);

		if (_physicsManager)
		_physicsManager->deleteJoint(_joint);
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

	Component* PhysicsJoint::getComponentA() const
	{
		return componentA;
	}

	Component* PhysicsJoint::getComponentB() const
	{
		return componentB;
	}

	bool PhysicsJoint::update()
	{
		return _physicsManager->updateJoint(this);
	}

	////////////////////////////////////////////////////////////////////////////////
	/// \brief JointGraphics::JointGraphics manages display of the joint objects
	/// \param physicsJoint
	/// \param parent
	///
	JointGraphics::JointGraphics(PhysicsJoint *physicsJoint, QGraphicsItem *parent)
		:QGraphicsPathItem(parent)
	{
		_physicsJoint = physicsJoint;
		setZValue(Z_JOINTVIEW);
	}

	QRectF JointGraphics::boundingRect() const
	{
		return QGraphicsPathItem::boundingRect().adjusted(-10,-10,10,10);
	}

	PhysicsJoint *JointGraphics::getPhysicsJoint() const
	{
		return _physicsJoint;
	}

	/*-------------------------------------------------------------------
	 * Pin joint graphics class
	 * */
	PinJointGraphics::PinJointGraphics(PhysicsJoint* physicsJoint, QGraphicsItem *parent)
		: JointGraphics(physicsJoint, parent)
	{
		setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
		setFlag(QGraphicsItem::ItemIgnoresParentOpacity, true);
		initializePainterPath();

		QPen _pen = QPen(Qt::blue);

		QRadialGradient radialGradient = QRadialGradient(QPointF(0,0), 15, QPointF(10,10));
		radialGradient.setColorAt(0, Qt::red);
		radialGradient.setColorAt(1, Qt::blue);
		QBrush _brush = QBrush(radialGradient);

		setPen(_pen);
		setBrush(_brush);
	}

	void PinJointGraphics::initializePainterPath()
	{
		if (_physicsJoint == NULL) return;
		if (_physicsJoint->jointType() != e_revoluteJoint) return;

		_painterPath = QPainterPath();
		float radius = 15;
		_painterPath.moveTo(0,0);
		_painterPath.addEllipse(QPointF(0,0), radius, radius);
		_painterPath.moveTo(0,0);

		radius += 15;
		b2RevoluteJoint* revoluteJoint = static_cast<b2RevoluteJoint*>(_physicsJoint->joint());
		if (revoluteJoint->IsMotorEnabled())
		{
			_painterPath.addText(QPointF(0,0),
								 QFont("Times", 12),
								 QString("Motor enabled"));
		} else {
			_painterPath.addText(QPointF(0,0),
								 QFont("Times", 12),
								 QString("Pivot"));
		}

		setPath(_painterPath);
	}
}
