#include "physicsjoint.h"
#include "physicsmanager.h"
#include "QsLog.h"
#include <QRadialGradient>
#include <QFont>
#include <QString>

namespace CDI
{
	/*-------------------------------------------------------------------------
	 * cdPinJoint class member definitions
	 * ----------------------------------------------------------------------*/
	cdPinJoint::cdPinJoint()
		: cdJoint(e_revoluteJoint)
	{
		_physicsmanager = nullptr;
		_jointDef		= new b2RevoluteJointDef;
		_joint			= nullptr;
		_componentA		= nullptr;
		_componentB		= nullptr;
	}

	cdPinJoint::~cdPinJoint()
	{
		if (_jointDef)		delete _jointDef;
		if (_componentA)	_componentA->removeJoint(this);
		if (_componentB)	_componentB->removeJoint(this);

		if (_physicsmanager && _joint)
			_physicsmanager->deleteJoint(_joint);
	}

	b2RevoluteJoint* cdPinJoint::joint() const
	{
		return _joint;
	}

	b2RevoluteJointDef* cdPinJoint::jointDef() const
	{
		return _jointDef;
	}

	Component *cdPinJoint::componentA() const
	{
		return _componentA;
	}

	Component *cdPinJoint::componentB() const
	{
		return _componentB;
	}

	Point2D cdPinJoint::relPosA() const
	{
		return _relPosA;
	}

	Point2D cdPinJoint::relPosB() const
	{
		return _relPosB;
	}

	/*-------------------------------------------------------------------------
	 * cdPinJoint class member definitions
	 * ----------------------------------------------------------------------*/

	cdSliderJoint::cdSliderJoint()
		:cdJoint(e_prismaticJoint)
	{
		_physicsmanager = nullptr;
		_jointDef		= new b2PrismaticJointDef;
		_joint			= nullptr;
		_componentA		= nullptr;
		_componentB		= nullptr;
	}

	cdSliderJoint::~cdSliderJoint()
	{

	}

	b2PrismaticJoint* cdSliderJoint::joint() const
	{
		return _joint;
	}

	b2PrismaticJointDef* cdSliderJoint::jointDef() const
	{
		return _jointDef;
	}

	Component *cdSliderJoint::componentA() const
	{
		return _componentA;
	}

	Component *cdSliderJoint::componentB() const
	{
		return _componentB;
	}

	Point2D cdSliderJoint::relPosA() const
	{
		return _relPosA;
	}

	Point2D cdSliderJoint::relPosB() const
	{
		return _relPosB;
	}

	/*-------------------------------------------------------------------------
	 * cdWeldJoint class member definitions
	 * ----------------------------------------------------------------------*/
	cdWeldJoint::cdWeldJoint()
		: cdJoint(e_weldJoint)
	{
		_physicsmanager		= nullptr;
		_jointDef			= new b2WeldJointDef;
		_joint				= nullptr;
		_componentA			= nullptr;
		_componentB			= nullptr;
		_relPosA			= Point2D();
		_relPosB			= Point2D();
	}

	cdWeldJoint::~cdWeldJoint()
	{

	}

	b2WeldJoint* cdWeldJoint::joint() const
	{
		return _joint;
	}

	b2WeldJointDef* cdWeldJoint::jointDef() const
	{
		return _jointDef;
	}

	Component *cdWeldJoint::componentA() const
	{
		return _componentA;
	}

	Component *cdWeldJoint::componentB() const
	{
		return _componentB;
	}

	Point2D cdWeldJoint::relPosA() const
	{
		return _relPosA;
	}

	Point2D cdWeldJoint::relPosB() const
	{
		return _relPosB;
	}

	/*-------------------------------------------------------------------------
	 * PhysicsJoint class member definitions
	 * ----------------------------------------------------------------------*/
	/*
	// Some of the functions which need to be used locally.
	// Do not add these as class member because we want to
	// reduce dependency on physics engine in this part
	*/

	PhysicsJoint::PhysicsJoint(b2JointType jointType, QGraphicsItem *parent)
		: QGraphicsPathItem(parent)
	{
		// created item is emoty by default
		setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
		setFlag(QGraphicsItem::ItemIgnoresParentOpacity, true);

		_id = uniqueHash();

		switch (jointType)
		{
		case e_revoluteJoint :
		{
			_cdJoint = new cdPinJoint();

			QPen _pen = QPen(Qt::blue);

			QRadialGradient radialGradient = QRadialGradient(QPointF(0,0), 15, QPointF(10,10));
			radialGradient.setColorAt(0, Qt::red);
			radialGradient.setColorAt(1, Qt::blue);
			QBrush _brush = QBrush(radialGradient);

			setPen(_pen);
			setBrush(_brush);
			break;
		}
		case e_prismaticJoint :
		{
			_cdJoint = new cdSliderJoint();

			QPen _pen = QPen(Qt::green);
			_pen.setWidth(4);
			setPen(_pen);
			break;
		}
		default:
			break;
		}
	}

	PhysicsJoint::~PhysicsJoint()
	{
		if (_cdJoint) delete _cdJoint;	// It will handle itself
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

	bool PhysicsJoint::updateJoint()
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

		b2RevoluteJointDef* revoluteJointDef =
				static_cast<b2RevoluteJointDef*>(_physicsJoint->jointDef());
		if (revoluteJointDef->enableMotor)
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
