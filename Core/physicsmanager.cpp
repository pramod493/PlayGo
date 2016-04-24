#include <QList>
#include "QsLog.h"
#include "physicsmanager.h"
#include "page.h"
#include "component.h"
#include "cdiboxdebugdraw.h"
#include "physicsjoint.h"

namespace CDI
{
	void PhysicsManager::setEnableDebugView(bool enableDebugView)
	{
		_enableDebugView = enableDebugView;
	}

	bool PhysicsManager::enableDebugView() const
	{
		return _enableDebugView;
	}

	PhysicsManager::PhysicsManager(PhysicsSettings& settings, QObject *parent) : QObject(parent)
	{
		defaultPhysicsScale = getPhysicsScale();

		_jointList = QList<cdJoint*>();

		_settings = settings;
		_enableDebugView = true;
		init();
	}

	PhysicsManager::PhysicsManager(QObject *parent)
		: QObject(parent)
	//: PhysicsManager(PhysicsSettings(), parent)
	{
		defaultPhysicsScale = getPhysicsScale();

		_jointList = QList<cdJoint*>();

		_settings = PhysicsSettings();
		_enableDebugView = true;
		init();
	}

	PhysicsManager::PhysicsManager(const PhysicsManager &physicsManager)
		: QObject(physicsManager.parent())
	{
		defaultPhysicsScale = getPhysicsScale();
		_jointList			= QList<cdJoint*>();
		_settings			= physicsManager._settings;
		_enableDebugView	= physicsManager._enableDebugView;
		init();
	}

	void PhysicsManager::init()
	{
		b2Vec2 gravity(_settings.gravity.x(), _settings.gravity.y());
		_b2World = new b2World(gravity);
		_b2World->SetAllowSleeping(_settings.enableSleep);

		debugView = new BoxDebugScene(defaultPhysicsScale);
		debugView->SetFlags(  b2Draw::e_shapeBit
							| b2Draw::e_pairBit
							| b2Draw::e_jointBit
							| b2Draw::e_centerOfMassBit);//b2Draw::e_aabbBit | b2Draw::e_centerOfMassBit);
		_b2World->SetDebugDraw(debugView);

		_contactListener = new cdContactListener(this);
		_b2World->SetContactListener(_contactListener);

		_internalLock = false;
		_isRunning = false;
		_quickpause = false;
		timer = NULL;
	}

	PhysicsManager::~PhysicsManager()
	{
		if (_b2World) delete _b2World;
		if (_contactListener) delete _contactListener;

		for (auto joint : _jointList)
			delete joint;

		// Remove physics component from all bodies
		QList<b2Body*> bodies = _box2DBodies.values();
		for(auto boxbody : bodies)
		{
			if (auto component = static_cast<Component*>(boxbody->GetUserData()))
			{
				component->setPhysicsBody(nullptr);
			}
		}
	}

	void PhysicsManager::destroyBody(Component *component)
	{
		destroyBody(component->physicsBody());
		component->setPhysicsBody(nullptr);
	}

	cdPinJoint *PhysicsManager::createPinJoint(Component *c1, Component *c2, QPointF scenePos,
											bool enableMotor, bool enableLimits,
											float motorSpeed, float motorTorque,
											float lowerLimit, float upperLimit)
	{
		cdPinJoint* pinJoint = new cdPinJoint(c1);
		c1->addToComponent(pinJoint);		// why?
		pinJoint->setPos(c1->mapFromScene(scenePos));
		pinJoint->_physicsmanager = this;	// call for everything

		b2RevoluteJointDef* jointDef  = pinJoint->jointDef();
		jointDef->bodyA = c1->physicsBody();
		jointDef->bodyB = c2->physicsBody();


		QPointF localPosA = c1->mapFromScene(scenePos) * c1->scale();
		QPointF localPosB = c2->mapFromScene(scenePos) * c2->scale();

		jointDef->localAnchorA.Set(localPosA.x()/defaultPhysicsScale,
								  localPosA.y()/defaultPhysicsScale);
		jointDef->localAnchorB.Set(localPosB.x()/defaultPhysicsScale,
								  localPosB.y()/defaultPhysicsScale);

		jointDef->enableLimit = enableLimits;
		jointDef->upperAngle = upperLimit * TO_RADIANS_FROM_DEG;
		jointDef->lowerAngle = lowerLimit * TO_RADIANS_FROM_DEG;

		jointDef->enableMotor = enableMotor;
		jointDef->motorSpeed = motorSpeed * TO_RAD_SEC_FROM_RPM;
		jointDef->maxMotorTorque = motorTorque;

		pinJoint->_joint = static_cast<b2RevoluteJoint *>(createJoint(*jointDef));

		pinJoint->_componentA = c1;
		pinJoint->_componentB = c2;
		pinJoint->_relPosA = c1->mapFromScene(scenePos);
		pinJoint->_relPosB = c2->mapFromScene(scenePos);

		pinJoint->_joint->SetUserData(static_cast<void*>(pinJoint));

		c1->addJoint(pinJoint);
		c2->addJoint(pinJoint);

		_jointList.push_back(pinJoint);

		pinJoint->initializeShape();

		if (!isMotorEnabled())
			pinJoint->joint()->EnableMotor(false);		// All motors are started as disabled

		return pinJoint;
	}

	cdSliderJoint *PhysicsManager::createPrismaticJoint(Component *c1, Component *c2,
											   QPointF startPos, QPointF endPos,
											   bool enableMotor, bool enableLimits,
											   float motorSpeed, float motorForce)
	{
		// create new joint
		cdSliderJoint* sliderJoint = new cdSliderJoint(c1);
		c1->addToComponent(sliderJoint);
		sliderJoint->setPos(c1->mapFromScene(startPos));
		sliderJoint->_physicsmanager = this;

		// Add components
		auto jointDef = sliderJoint->jointDef();
		jointDef->bodyA = c1->physicsBody();
		jointDef->bodyB = c2->physicsBody();
		jointDef->collideConnected = false;

		// Define anchor points
		QPointF localPosA = c1->mapFromScene(startPos) * c1->scale();
		QPointF localPosB = c2->mapFromScene(startPos) * c2->scale();

		jointDef->localAnchorA.Set(localPosA.x()/defaultPhysicsScale,
								  localPosA.y()/defaultPhysicsScale);
		jointDef->localAnchorB.Set(localPosB.x()/defaultPhysicsScale,
								  localPosB.y()/defaultPhysicsScale);

		// \todo Check if the reference angle is correct
		jointDef->referenceAngle =
				jointDef->bodyA->GetAngle() - jointDef->bodyB->GetAngle();

		// Set up axis
		auto localStart = c1->mapFromScene(startPos) * c1->scale();
		auto localEnd   = c1->mapFromScene(endPos) * c1->scale();
		auto lineVector = QVector2D(localEnd-localStart);
		jointDef->localAxisA.Set(lineVector.x(), lineVector.y());
		jointDef->localAxisA.Normalize();

		// Joint limit definition
		jointDef->enableLimit = enableLimits;
		if (enableLimits)
		{
			jointDef->lowerTranslation = 0;
			jointDef->upperTranslation = lineVector.length()/defaultPhysicsScale;
		}

		// Define motor params
		jointDef->enableMotor = enableMotor;
		jointDef->motorSpeed = motorSpeed;
		jointDef->maxMotorForce = motorForce;

		// create joint in box2d
		sliderJoint->_joint = static_cast<b2PrismaticJoint*>(createJoint(*jointDef));

		sliderJoint->_componentA = c1;
		sliderJoint->_componentB = c2;

		sliderJoint->_relPosA = c1->mapFromScene(startPos);
		sliderJoint->_relPosB = c2->mapFromScene(startPos);

		c1->addJoint(sliderJoint);
		c2->addJoint(sliderJoint);
		_jointList.push_back(sliderJoint);

		sliderJoint->initializeShape();

		if (!isMotorEnabled())
			sliderJoint->joint()->EnableMotor(false);

		// Connect to change in component trasnformation.  Actually we should
		// have updated the joint instead. But that is for later
		connect(c1, SIGNAL(onTransformChange(QGraphicsItem*)),
				sliderJoint, SLOT(initializeShape()));
		return sliderJoint;
	}

	bool PhysicsManager::updateJoint(cdJoint *physicsJoint)
	{
		switch (physicsJoint->jointType())
		{
		case e_revoluteJoint :
			// Keep it east to copy
			if (auto joint = dynamic_cast<cdPinJoint*>(physicsJoint))
			{
				auto localPosA = joint->_relPosA * joint->_componentA->scale();
				auto localPosB = joint->_relPosB * joint->_componentB->scale();

				auto jointDef = joint->jointDef();

				jointDef->localAnchorA.Set
						(localPosA.x()/defaultPhysicsScale, localPosA.y()/defaultPhysicsScale);
				jointDef->localAnchorB.Set
						(localPosB.x()/defaultPhysicsScale, localPosB.y()/defaultPhysicsScale);

				deleteJoint(joint->joint());
				auto newb2Joint = static_cast<b2RevoluteJoint*>(createJoint(*jointDef));
				// Do not copy params. This is why we are creating a new joint
				newb2Joint->SetUserData(static_cast<void*>(joint));
				joint->_joint = newb2Joint;

				// If motors are currently disabled, set it to disable as well
				if (!isMotorEnabled() && newb2Joint->IsMotorEnabled())
					newb2Joint->EnableMotor(false);
				return true;
			}
		case e_prismaticJoint :
			break;
		default:
			break;
		}

		return false;
	}

	bool PhysicsManager::moveJoint(cdJoint *joint, QPointF newScenePos)
	{
		switch (joint->jointType()) {
		case e_revoluteJoint:
			if (auto cast_joint = dynamic_cast<cdPinJoint*>(joint))
			{
				cast_joint->_relPosA = cast_joint->_componentA->mapFromScene(newScenePos);
				cast_joint->_relPosB = cast_joint->_componentB->mapFromScene(newScenePos);
			}
			break;
		default:
			break;
		}
		// update the joint regardless of success in position update
		return updateJoint(joint);
	}
	bool PhysicsManager::deleteJoint(cdJoint *joint)
	{
		if (_jointList.contains(joint))
			_jointList.removeOne(joint);
		deleteJoint(joint->joint());
		return true;
	}

	/*-------------------------------------------------------------------------
	 *  Protected functions for creating/deleting entities in Box2D
	 * Not to be used directly
	 *-----------------------------------------------------------------------*/
	b2Body* PhysicsManager::createBody(const b2BodyDef &def)
	{
		if (_b2World == nullptr)
			return nullptr;

		b2Body* body = _b2World->CreateBody(&def);
		return body;
	}

	void PhysicsManager::destroyBody(b2Body *body)
	{
		_b2World->DestroyBody(body);
	}


	b2Joint *PhysicsManager::createJoint(b2JointDef &jointDef)
	{
		b2Joint* joint = _b2World->CreateJoint(&jointDef);
		return joint;
	}

	bool PhysicsManager::deleteJoint(b2Joint *joint)
	{
		if (joint == nullptr) return false;
		_b2World->DestroyJoint(joint);
		return true;
	}

	void PhysicsManager::updateSettings(PhysicsSettings* newSettings)
	{
		// Update settings which don't need further work.
		_settings.timeStep = newSettings->timeStep;
		_settings.velocityIterations = newSettings->velocityIterations;
		_settings.positionIterations = newSettings->positionIterations;

		// Update settings which need to be updated to b2World
		if (!qFuzzyCompare(_settings.gravity.x(), newSettings->gravity.x())
				|| !qFuzzyCompare(_settings.gravity.y(), newSettings->gravity.y()))
		{
			_settings.gravity = newSettings->gravity;
			if (_b2World != NULL) _b2World->SetGravity
					(b2Vec2(_settings.gravity.x(), _settings.gravity.y()));
		}

		if (_settings.enableSleep != newSettings->enableSleep)
		{
			_settings.enableSleep = newSettings->enableSleep;
			if (_b2World != NULL)
				_b2World->SetAllowSleeping(_settings.enableSleep);
		}
	}

	void PhysicsManager::pause()
	{
		if (timer!= NULL) timer->stop();
		_isRunning = false;

		if (debugView) debugView->clear();
	}

	void PhysicsManager::start(int timerStepIn_msecs)
	{
		if (timer != NULL)
		{
			delete timer;
		}

		// Start the timer for physics engine
		timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()),
				this, SLOT(step()));
		timer->start(timerStepIn_msecs);
		_quickpause = false;
		_isRunning = true;
		if (debugView) debugView->clear();
	}

	void PhysicsManager::step()
	{
		if (_b2World == NULL || _quickpause) return;

		emit physicsStepStart();
		_b2World->Step(_settings.timeStep, _settings.velocityIterations,
					   _settings.positionIterations);
		if (_enableDebugView)
		{
			debugView->clear();
			_b2World->DrawDebugData();
		}
		emit physicsStepComplete();

		if (false){
			for(auto joint : _jointList)
			{
				if(auto pinjoint = dynamic_cast<cdPinJoint*>(joint))
				{
//					qDebug() <<"Ref:" << pinjoint->referenceAngle()
//							<< "Ang:" << pinjoint->jointAngle()
//							<< "Vel:" << pinjoint->motorSpeed();
					b2RevoluteJoint* b2_pinjoint = pinjoint->joint();
					if (b2_pinjoint->IsLimitEnabled() && b2_pinjoint->IsMotorEnabled())
					{
						if (b2_pinjoint->GetJointAngle() < b2_pinjoint->GetLowerLimit() ||
								b2_pinjoint->GetJointAngle() > b2_pinjoint->GetUpperLimit())
						{
							/* Reached limits. Reverse the direction of torque on this one
							 * qDebug()<< "@limits" <<
									   b2_pinjoint->GetJointAngle() <<
									   b2_pinjoint->GetLowerLimit() <<
									   b2_pinjoint->GetUpperLimit() <<
									   pinjoint->jointDef()->motorSpeed <<
									   b2_pinjoint->GetMotorSpeed();*/
							if (b2_pinjoint->GetJointAngle() < b2_pinjoint->GetLowerLimit())
								b2_pinjoint->SetMotorSpeed(abs(pinjoint->jointDef()->motorSpeed));
							else
								b2_pinjoint->SetMotorSpeed(-abs(pinjoint->jointDef()->motorSpeed));
						}
					}
				}
			}
		}
	}

	void PhysicsManager::quickPause(bool enable)
	{
		return;
		_quickpause = enable;
	}

	void PhysicsManager::setEnableGravity(bool enable)
	{
		// Check if it is alredy in requested state
		if (enable == _settings.enableGravity) return;
		_b2World->SetGravity(enable ?
							 b2Vec2(_settings.gravity.x(), _settings.gravity.y()) :
								 b2Vec2(0,0));
		_settings.enableGravity = enable;
	}

	bool PhysicsManager::isGravityEnabled() const
	{
		return _settings.enableGravity;
	}

	void PhysicsManager::setEnableMotor(bool enable)
	{
		if (_settings.enableMotor == enable) return;	// Nothing to do
		_settings.enableMotor = enable;

		auto stopmotion = [](cdJoint *c)
		{
			if (auto bodyA = c->joint()->GetBodyA())
			{
				bodyA->SetLinearVelocity(b2Vec2(0,0));
				bodyA->SetAngularVelocity(0);
			}
			if (auto bodyB = c->joint()->GetBodyB())
			{
				bodyB->SetLinearVelocity(b2Vec2(0,0));
				bodyB->SetAngularVelocity(0);
			}
		};

		for (cdJoint* physicsJoint: _jointList)
		{
			switch (physicsJoint->jointType())
			{
				case e_revoluteJoint :
				{
					if (cdPinJoint* pinjoint = dynamic_cast<cdPinJoint*>(physicsJoint))
					{
						b2RevoluteJoint* b2PinJoint =	pinjoint->joint();
						b2RevoluteJointDef* b2PinJointDef = pinjoint->jointDef();
						enable ? b2PinJoint->EnableMotor(b2PinJointDef->enableMotor) : b2PinJoint->EnableMotor(false);
					} // Note- We can eliminate switch statements if we are using dynamic cast
					break;
				}
				case e_prismaticJoint :
				{

					/*b2PrismaticJoint* sliderJoint = static_cast<b2PrismaticJoint*>(physicsJoint->_joint);
					b2PrismaticJointDef* sliderJointDef = static_cast<b2PrismaticJointDef*>(physicsJoint->_jointDef);
					enable ? sliderJoint->EnableMotor(sliderJointDef->enableMotor) : sliderJoint->EnableMotor(false);*/
					break;
				}
			default:
				break;
			}

			if (!enable)	// pause objects when disabling the motor
				stopmotion(physicsJoint);
//			{
//				if (auto bodyA = physicsJoint->joint()->GetBodyA())
//				{
//					bodyA->SetLinearVelocity(b2Vec2(0,0));
//					bodyA->SetAngularVelocity(0);
//				}
//				if (auto bodyB = physicsJoint->joint()->GetBodyB())
//				{
//					bodyB->SetLinearVelocity(b2Vec2(0,0));
//					bodyB->SetAngularVelocity(0);
//				}
//			}
		}
	}

	bool PhysicsManager::isMotorEnabled() const
	{
		return _settings.enableMotor;
	}

	void PhysicsManager::setGlobalCollision
			(QList<Component*> components, bool enableCollision)
	{
		for(Component* component : components)
		{
			b2Body* body = component->physicsBody();
			body->playgoCollisionEnabled = enableCollision;
		}
	}

	void PhysicsManager::updateComponentPosition(QList<Component *> &components)
	{
		for(auto component: components)
			updateComponentPosition(component);
	}

	void PhysicsManager::updateComponentPosition(Component* component)
	{
		if (component->physicsBody())
		{
			b2Body* body = component->physicsBody();
			b2Vec2 pos = body->GetPosition();
			float rotation = body->GetAngle();
			_internalLock = true;
			component->setPos(pos.x * defaultPhysicsScale, pos.y * defaultPhysicsScale);
			component->setRotation(rotation * (180.0f/_PI_));
			_internalLock = false;
		}
	}

	void PhysicsManager::updateFromComponentPosition(QList<Component *> &components)
	{
		for(Component* component : components)
		{
			if (component->requiresRegeneration || component->pendingPositionUpdate)
				updateFromComponentPosition(component);
			else
				QLOG_WARN()
						<< "Why are we updating components which don't require any update";
		}
	}

	void PhysicsManager::updateFromComponentPosition(Component *component)
	{
		if (component->physicsBody() && !_internalLock)
		{
			if (component->requiresRegeneration ||
				!cdCompare(component->previousScale, component->scale()))
				component->regenerateInternals();

			// TODO if any of the joints are connected to fixed object, update the joint position

			b2Body* body = component->physicsBody();
			Point2D pos = component->pos();
			float rotation = component->rotation() * _PI_/180.0f;
			b2Vec2 boxPos = b2Vec2(pos.x()/defaultPhysicsScale, pos.y()/defaultPhysicsScale);
			//b2Vec2 currentPos = body->GetPosition();
			body->SetTransform(boxPos, rotation);
			body->SetAwake(true);
		}
	}

	void PhysicsManager::onItemAdd(QGraphicsItem* item)
	{
		// Reference
		// http://www.iforce2d.net/b2dtut/bodies
		if (item->type() != Component::Type) return;
		Component* component = qgraphicsitem_cast<Component*>(item);
		addPhysicsBody(component);
	}

	void PhysicsManager::addPhysicsBody(Component *component)
	{
		if (component->physicsBody()) return;

		b2BodyDef  def;
		def.type = b2_dynamicBody;
		Point2D pos = component->pos();
		def.position = b2Vec2(pos.x()/defaultPhysicsScale, pos.y()/defaultPhysicsScale);
		def.angle = component->rotation() * TO_RADIANS_FROM_DEG;	// Angle in radians
		b2Body* newBody =_b2World->CreateBody(&def);
		newBody->SetUserData(static_cast<void*>(component));
		component->setPhysicsBody(newBody);
	}
}
