#include "physicsjoint.h"
#include "physicsmanager.h"
#include "QsLog.h"
#include <QRadialGradient>
#include <QFont>
#include <QString>

#include <QMessageBox>

namespace CDI
{
	/*-------------------------------------------------------------------------
	 * cdPinJoint class member definitions
	 * ----------------------------------------------------------------------*/
	cdPinJoint::cdPinJoint(QGraphicsItem *parent)
		: cdJoint(e_revoluteJoint, parent)
	{
		_physicsmanager = nullptr;
		_jointDef		= new b2RevoluteJointDef;
		_joint			= nullptr;
		_componentA		= nullptr;
		_componentB		= nullptr;

		// created item is emoty by default
		setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
		setFlag(QGraphicsItem::ItemIgnoresParentOpacity, true);
		// Need not set this since it will anyways be on top of the component
		setZValue(Z_JOINTVIEW);

		// Set up the draw options for the pin joint
		QPen _pen = QPen(Qt::black);
		_pen.setWidth(1);

		QRadialGradient radialGradient = QRadialGradient(QPointF(0,0), 15, QPointF(10,10));
		radialGradient.setColorAt(0, Qt::red);
		radialGradient.setColorAt(1, Qt::blue);

		QLinearGradient lineargradient = QLinearGradient(QPointF(0,0), QPointF(1,1));
		lineargradient.setColorAt(0, Qt::yellow);
		lineargradient.setColorAt(1, Qt::blue);
		lineargradient.setSpread(QGradient::PadSpread);
		lineargradient.setCoordinateMode(QGradient::ObjectBoundingMode);

		setPen(_pen);
		setBrush(QBrush(lineargradient));
	}

	cdPinJoint::~cdPinJoint()
	{
		emit onJointDelete(this);
		if (_jointDef) delete _jointDef;
		if (_componentA) _componentA->removeJoint(this);
		if (_componentB) _componentB->removeJoint(this);
		// Automatically calls the physics manager for deletion
		if (_physicsmanager && _joint)
			_physicsmanager->deleteJoint(this);
	}

	b2RevoluteJoint* cdPinJoint::joint() const
	{
		return _joint;
	}

	b2RevoluteJointDef* cdPinJoint::jointDef() const
	{
		return _jointDef;
	}

	void cdPinJoint::updateJoint()
	{
		if (_physicsmanager == nullptr ||  _joint == nullptr)
		{
			QLOG_ERROR() << "Trying to update empmty joint";
			return;
		}
		// TODO - Implement this function
		_physicsmanager->updateJoint(this);
		initializeShape();
	}

	void cdPinJoint::initializeShape()
	{
		// NOTE - Check if the object is on component A or B and render accordingly
		// update the render function
		QPainterPath painterpath;

		float radius = 15;
		painterpath.moveTo(0,0);
		painterpath.addEllipse(-radius, -radius, 2*radius, 2*radius);

		QString text = (_jointDef->enableMotor) ?
					QString("Motor") : QString("Pin");
		painterpath.addText(QPointF(1.5f*radius, 0),QFont("Times", 12), text);
		setPath(painterpath);
		setZValue(Z_JOINTVIEW);
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

	bool  cdPinJoint::isMotorEnabled() const
	{
		return _joint->IsMotorEnabled();
	}

	float cdPinJoint::maxMotorTorque() const
	{
		return _joint->GetMaxMotorTorque();
	}

	float cdPinJoint::motorSpeed() const
	{
		return _joint->GetMotorSpeed() * TO_RPM_FROM_RAD_SEC;
	}

	float cdPinJoint::isLimitsEnabled() const
	{
		return _joint->IsLimitEnabled();
	}

	float cdPinJoint::lowerLimit() const
	{
		return _joint->GetLowerLimit() * TO_DEGREES_FROM_RAD;
	}

	float cdPinJoint::upperLimit() const
	{
		return _joint->GetUpperLimit() * TO_DEGREES_FROM_RAD;
	}

	float cdPinJoint::referenceAngle() const
	{
		return _joint->GetReferenceAngle() * TO_DEGREES_FROM_RAD;
	}

	float cdPinJoint::jointAngle() const
	{
		return _joint->GetJointAngle() * TO_DEGREES_FROM_RAD;
	}

	/*-------------------------------------------------------------------------
	 * cdPinJoint class member definitions
	 * ----------------------------------------------------------------------*/

//	cdSliderJoint::cdSliderJoint()
//		:cdJoint(e_prismaticJoint)
//	{
//		_physicsmanager = nullptr;
//		_jointDef		= new b2PrismaticJointDef;
//		_joint			= nullptr;
//		_componentA		= nullptr;
//		_componentB		= nullptr;
//	}

//	cdSliderJoint::~cdSliderJoint()
//	{

//	}

//	b2PrismaticJoint* cdSliderJoint::joint() const
//	{
//		return _joint;
//	}

//	b2PrismaticJointDef* cdSliderJoint::jointDef() const
//	{
//		return _jointDef;
//	}

//	Component *cdSliderJoint::componentA() const
//	{
//		return _componentA;
//	}

//	Component *cdSliderJoint::componentB() const
//	{
//		return _componentB;
//	}

//	Point2D cdSliderJoint::relPosA() const
//	{
//		return _relPosA;
//	}

//	Point2D cdSliderJoint::relPosB() const
//	{
//		return _relPosB;
//	}

//	/*-------------------------------------------------------------------------
//	 * cdWeldJoint class member definitions
//	 * ----------------------------------------------------------------------*/
//	cdWeldJoint::cdWeldJoint()
//		: cdJoint(e_weldJoint)
//	{
//		_physicsmanager		= nullptr;
//		_jointDef			= new b2WeldJointDef;
//		_joint				= nullptr;
//		_componentA			= nullptr;
//		_componentB			= nullptr;
//		_relPosA			= Point2D();
//		_relPosB			= Point2D();
//	}

//	cdWeldJoint::~cdWeldJoint()
//	{

//	}

//	b2WeldJoint* cdWeldJoint::joint() const
//	{
//		return _joint;
//	}

//	b2WeldJointDef* cdWeldJoint::jointDef() const
//	{
//		return _jointDef;
//	}

//	Component *cdWeldJoint::componentA() const
//	{
//		return _componentA;
//	}

//	Component *cdWeldJoint::componentB() const
//	{
//		return _componentB;
//	}

//	Point2D cdWeldJoint::relPosA() const
//	{
//		return _relPosA;
//	}

//	Point2D cdWeldJoint::relPosB() const
//	{
//		return _relPosB;
//	}

//	/*-------------------------------------------------------------------------
//	 * PhysicsJoint class member definitions
//	 * ----------------------------------------------------------------------*/
//	/*
//	// Some of the functions which need to be used locally.
//	// Do not add these as class member because we want to
//	// reduce dependency on physics engine in this part
//	*/

//	PhysicsJoint::PhysicsJoint(b2JointType jointType, QGraphicsItem *parent)
//		: QGraphicsPathItem(parent)
//	{

//		_id = uniqueHash();

//		switch (jointType)
//		{
//		case e_revoluteJoint :
//		{
//			_cdJoint = new cdPinJoint();

//			break;
//		}
//		case e_prismaticJoint :
//		{
//			_cdJoint = new cdSliderJoint();

//			QPen _pen = QPen(Qt::green);
//			_pen.setWidth(4);
//			setPen(_pen);
//			break;
//		}
//		default:
//			break;
//		}
//	}

//	PhysicsJoint::~PhysicsJoint()
//	{
//		if (_cdJoint) delete _cdJoint;	// It will handle itself
//	}

//	QUuid PhysicsJoint::id() const
//	{
//		return _id;
//	}

//	bool PhysicsJoint::updateJoint()
//	{
//		return false;//_physicsManager->updateJoint(this);
//	}

//	QRectF PhysicsJoint::boundingRect() const
//	{
//		return QGraphicsPathItem::boundingRect().adjusted(-10,-10,10,10);
//	}

//	cdJoint *PhysicsJoint::getcdjoint() const
//	{
//		return _cdJoint;
//	}

//	void PhysicsJoint::initializePainterPath()
//	{
//		if (_cdJoint->joint() == nullptr || _cdJoint->jointDef() == nullptr)
//		{
//			return;	// nothing to do here
//		}
//		switch(_cdJoint->jointType())
//		{
//		case e_revoluteJoint :
//		{
//			cdPinJoint* pinJoint = dynamic_cast<cdPinJoint*>(_cdJoint);
//			QPainterPath painterpath;

//			float radius = 15;
//			painterpath.moveTo(0,0);
//			painterpath.addEllipse(QPointF(0,0), radius, radius);
//			painterpath.moveTo(0,0);

//			radius += 15;
//			QString text = (pinJoint->jointDef()->enableMotor) ?
//						QString("Motor") : QString("Pin");
//			painterpath.addText(QPointF(0,0),QFont("Times", 12), text);
//			setPath(painterpath);
//			break;
//		}
//		case e_prismaticJoint :
//		{
//			break;
//		}
//		case e_gearJoint :
//		{
//			break;
//		}
//		case e_weldJoint :
//		{
//			break;
//		}
//		default:
//			break;
//		}
//	}
}
