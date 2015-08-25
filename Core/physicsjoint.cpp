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

		/*QRadialGradient radialGradient = QRadialGradient(QPointF(0,0), 15, QPointF(10,10));
		radialGradient.setColorAt(0, Qt::red);
		radialGradient.setColorAt(1, Qt::blue);*/

		QLinearGradient lineargradient = QLinearGradient(QPointF(0,0), QPointF(1,1));
		lineargradient.setColorAt(0, Qt::yellow);
		lineargradient.setColorAt(1, Qt::blue);
		lineargradient.setSpread(QGradient::PadSpread);
		lineargradient.setCoordinateMode(QGradient::ObjectBoundingMode);

		setPen(_pen);
		setBrush(QBrush(lineargradient));

		// Set the description text as separate object because it fucks up the
		// rendering due to mix of pen and brush
		_descriptionText = new QGraphicsSimpleTextItem(this);
		_descriptionText->setPos(30,0);
		_descriptionText->setBrush(QBrush(Qt::black));
		_descriptionText->setScale(2.5f);
		_descriptionText->setFlag(QGraphicsItem::ItemIgnoresTransformations, false);

		_motorDirectionItem = new Stroke(this);
		_motorDirectionItem->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
		_pen.setWidth(5);
		_pen.setColor(Qt::red);
		_motorDirectionItem->setPen(_pen);
		_motorDirectionItem->setPos(0,0);

		/*_partDirectionLine = new QGraphicsLineItem(this);
		_partDirectionLine->setPos(0,0);
		_partDirectionLine->setLine(0,0,100,0);*/
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
		// No need to delete description text
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

		if (_jointDef->enableLimit)
		{
			// show limits
			int length_marker = 30;
			QPointF lowerLimit = QPointF
					(length_marker * cos(_jointDef->lowerAngle),
					 length_marker * sin(_jointDef->lowerAngle));
			QPointF upperLimits = QPointF
					(length_marker * cos(_jointDef->upperAngle),
					 length_marker * sin(_jointDef->upperAngle));

			painterpath.moveTo(0,0);
			painterpath.lineTo(lowerLimit);
			painterpath.lineTo(0,0);
			painterpath.lineTo(upperLimits);
		}

		setPath(painterpath);

		//QString text = (_jointDef->enableMotor)?QString("Motor"):QString("Pin");
		// No need of text as we are now showing direction
		//_descriptionText->setText(text);

		/* Does not work as expected.
		 * Need to get the right angle w/ parent item's children
		 * if (_componentA)
		{
			QPen tmpPen = QPen(); tmpPen.setWidth(5);
			_partDirectionLine->setPen(tmpPen);
			_partDirectionLine->setParentItem(0);
			_partDirectionLine->setPos(mapToScene(QPointF(0,0)));
			_partDirectionLine->setRotation(_jointDef->lowerAngle * TO_DEGREES_FROM_RAD);
			//_partDirectionLine->setRotation(jointAngle());
			//_componentA->addToComponent(_partDirectionLine);
		}*/

		if (_jointDef->enableMotor)
		{
			QVector<Point2DPT*> points;
			int curveRad= radius + 5;
			points.reserve(180/5);
			float interval = 5.0f * TO_RADIANS_FROM_DEG;
			if (_jointDef->maxMotorTorque > 0)
				for (float theta=0; theta < _PI_; theta+=interval)
					points.push_back
							(new Point2DPT(curveRad*cos(theta), curveRad*sin(theta), 1.0f-theta/_PI_, 0));
			else
				for (float theta=0; theta < _PI_; theta+=interval)
					points.push_back
							(new Point2DPT(curveRad*cos(theta), curveRad*sin(theta), theta/_PI_, 0));
			_motorDirectionItem->setStrokePath(points);
			_motorDirectionItem->show();
		}
		else
		{
			_motorDirectionItem->hide();
		}
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

	void cdPinJoint::enableLimits(bool enable, float lowerAngle, float upperAngle)
	{
		_jointDef->enableLimit= enable;
		_joint->EnableLimit(enable);
		if (enable)
		{
			qDebug() << "Setting limits to " << lowerAngle << upperAngle;
			_joint->SetLimits(lowerAngle * TO_RADIANS_FROM_DEG, upperAngle * TO_RADIANS_FROM_DEG);
			_jointDef->lowerAngle = lowerAngle * TO_RADIANS_FROM_DEG;
			_jointDef->upperAngle = upperAngle * TO_RADIANS_FROM_DEG;
		}

		initializeShape();
	}

	bool cdPinJoint::isLimitsEnabled() const
	{
		return _joint->IsLimitEnabled();
	}

	float cdPinJoint::lowerLimit() const
	{
		return _jointDef->lowerAngle * TO_DEGREES_FROM_RAD;
		//return _joint->GetLowerLimit() * TO_DEGREES_FROM_RAD;
	}

	float cdPinJoint::upperLimit() const
	{
		return _jointDef->upperAngle * TO_DEGREES_FROM_RAD;
		//return _joint->GetUpperLimit() * TO_DEGREES_FROM_RAD;
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

	cdSliderJoint::cdSliderJoint(QGraphicsItem* parent)
		:cdJoint(e_prismaticJoint, parent)
	{
		_physicsmanager = nullptr;
		_jointDef		= new b2PrismaticJointDef;
		_joint			= nullptr;
		_componentA		= nullptr;
		_componentB		= nullptr;

		// created item is emoty by default
		setFlag(QGraphicsItem::ItemIgnoresTransformations, false);	// not this time
		setFlag(QGraphicsItem::ItemIgnoresParentOpacity, true);
		// Need not set this since it will anyways be on top of the component
		setZValue(Z_JOINTVIEW);

		// Set up the draw options for the pin joint
		QPen _pen = QPen(Qt::black);
		_pen.setWidth(4);

		QLinearGradient lineargradient = QLinearGradient(QPointF(0,0), QPointF(1,1));
		lineargradient.setColorAt(0, Qt::yellow);
		lineargradient.setColorAt(1, Qt::blue);
		lineargradient.setSpread(QGradient::PadSpread);
		lineargradient.setCoordinateMode(QGradient::ObjectBoundingMode);

		setPen(_pen);
		setBrush(QBrush(lineargradient));

		_descriptionText = new QGraphicsSimpleTextItem(this);
		_descriptionText->setPos(15, 15);	// offset a little bit from the center
		_descriptionText->setBrush(Qt::black);
		_descriptionText->setScale(1.5f	);
		_descriptionText->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
		_descriptionText->setText(QString("Slider"));

		_motorDirectionItem		= nullptr;
	}

	cdSliderJoint::~cdSliderJoint()
	{
		emit onJointDelete(this);
		if (_jointDef) delete _jointDef;
		if (_componentA) _componentA->removeJoint(this);
		if (_componentB) _componentB->removeJoint(this);

		if (_physicsmanager && _joint)
			_physicsmanager->deleteJoint(this);
		_joint = nullptr;
	}

	b2PrismaticJoint* cdSliderJoint::joint() const
	{
		return _joint;
	}

	b2PrismaticJointDef* cdSliderJoint::jointDef() const
	{
		return _jointDef;
	}

	void cdSliderJoint::updateJoint()
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

	void cdSliderJoint::initializeShape()
	{
		QPainterPath painterpath;
		auto radius = 15;
		auto axis_length = 20;
		// No need to add extra widgets
		//painterpath.moveTo(0,0);
		//painterpath.addEllipse(-radius, -radius, 2*radius, 2*radius);

		QString text("Slider");
		if (_jointDef->enableMotor) text += QString(" w/ motor");
		_descriptionText->setText(text);

		if (_jointDef->enableLimit)
		{
			auto axis_start_point =
					QPointF(_jointDef->lowerTranslation * _jointDef->localAxisA.x * getPhysicsScale()/_componentA->scale(),
							_jointDef->lowerTranslation * _jointDef->localAxisA.y * getPhysicsScale()/_componentA->scale());

			auto axis_end_point =
					QPointF(_jointDef->upperTranslation * _jointDef->localAxisA.x * getPhysicsScale()/_componentA->scale(),
							_jointDef->upperTranslation * _jointDef->localAxisA.y * getPhysicsScale()/_componentA->scale());

			//auto axis_vector = QLineF(axis_start_point, axis_end_point);
			//auto axis_normal = axis_vector.normalVector();
			//axis_normal = axis_normal * 15 / axis_normal.length();	// reduce the length
			int length_cross = 10;
			auto drawCross = [&](QPointF pos)
			{
				painterpath.moveTo(pos-QPointF(0,length_cross));
				painterpath.lineTo(pos+ QPointF(0, length_cross));

				painterpath.moveTo(pos-QPointF(length_cross, 0));
				painterpath.lineTo(pos+ QPointF(length_cross, 0));
			};
			drawCross(axis_start_point);
			drawCross(axis_end_point);
			painterpath.moveTo(axis_start_point);
			painterpath.lineTo(axis_end_point);
		} else
		{
			QLOG_WARN() << "@" << id().toString() << " not supposed to create slider joint without limits";
			auto axis_end_point = QPointF
					(_jointDef->localAxisA.x * axis_length, _jointDef->localAxisA.y * axis_length);
			painterpath.moveTo(0,0);
			painterpath.lineTo(axis_end_point);
		}
		setPath(painterpath);
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

	Point2D cdSliderJoint::localAxisA() const
	{
		return QPointF();
	}

	bool cdSliderJoint::isMotorEnabled() const
	{
		return _joint->IsMotorEnabled();
	}

	float cdSliderJoint::maxMotorForce() const
	{
		return _joint->GetMaxMotorForce();
	}

	float cdSliderJoint::motorSpeed() const
	{
		// \todo Convert the speed to local coordinate units (maybe now)
		return _joint->GetMotorSpeed();
	}

	void cdSliderJoint::enableLimits(bool enable, float lowerLength, float upperLength)
	{

	}

	bool cdSliderJoint::isLimitsEnabled() const
	{
		return _joint->IsLimitEnabled();
	}

	float cdSliderJoint::lowerLimit() const
	{
		return _jointDef->lowerTranslation;
	}

	float cdSliderJoint::upperLimit() const
	{
		return _jointDef->upperTranslation;
	}

	float cdSliderJoint::referenceAngle() const
	{
		return _joint->GetReferenceAngle() * TO_DEGREES_FROM_RAD;
	}

	float cdSliderJoint::jointTranslation() const
	{
		return _joint->GetJointTranslation();
	}

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
