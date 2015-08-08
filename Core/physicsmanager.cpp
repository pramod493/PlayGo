#include <QList>
#include "physicsmanager.h"
#include "page.h"
#include <QDebug>
#include "QsLog.h"
#include "component.h"
#include "cdiboxdebugdraw.h"
#include "Box2D/Common/b2Draw.h"

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

	PhysicsManager::PhysicsManager(PhysicsSettings *settings, Page *parentPage) : QObject(parentPage)
	{
		defaultPhysicsScale = getPhysicsScale();

		_jointList = QList<PhysicsJoint*>();

		_settings.timeStep = settings->timeStep;
		_settings.velocityIterations = settings->velocityIterations;
		_settings.positionIterations = settings->positionIterations;
		_settings.gravity = settings->gravity;
		_settings.enableSleep = settings->enableSleep;

        _enableDebugView = true;
		init();
	}

	PhysicsManager::PhysicsManager(QObject *parent)
		:QObject(parent)
	{
		defaultPhysicsScale = getPhysicsScale();
		_settings = PhysicsSettings();
		_enableDebugView = true;

		init();
	}

	PhysicsManager::PhysicsManager(const PhysicsManager &physicsManager)
		:QObject(physicsManager.parent())
	{
		defaultPhysicsScale = getPhysicsScale();
		_settings = PhysicsSettings();
		_enableDebugView = physicsManager.enableDebugView();

		init();
	}

	void PhysicsManager::init()
	{
		b2Vec2 gravity(_settings.gravity.x(), _settings.gravity.y());
		_b2World = new b2World(gravity);
		_b2World->SetAllowSleeping(_settings.enableSleep);

		debugView = new BoxDebugScene(defaultPhysicsScale);
		debugView->SetFlags(  b2Draw::e_shapeBit
							  | b2Draw::e_pairBit | b2Draw::e_jointBit
							  | b2Draw::e_centerOfMassBit);//b2Draw::e_aabbBit | b2Draw::e_centerOfMassBit);
		_b2World->SetDebugDraw(debugView);

		_internalLock = false;
		_isRunning = false;
		timer = NULL;
	}

	PhysicsManager::~PhysicsManager()
	{
		if (_b2World!= NULL) delete _b2World;
	}

	b2Body* PhysicsManager::createBody(const b2BodyDef &def)
	{
		if (_b2World!= NULL)
		{
			b2Body* body = _b2World->CreateBody(&def);
			return body;
		}
		return NULL;
	}

	void PhysicsManager::destroyBody(b2Body *body)
	{
		_b2World->DestroyBody(body);
	}

	void PhysicsManager::destroyBody(Component *component)
	{
		destroyBody(component->physicsBody());
	}

	b2Joint *PhysicsManager::createJoint(b2JointDef &jointDef)
	{
		b2Joint* joint = _b2World->CreateJoint(&jointDef);
		return joint;
	}

	PhysicsJoint *PhysicsManager::createPinJoint(Component *c1, Component *c2, QPointF scenePos,
											bool enableMotor, bool enableLimits,
											float motorSpeed, float motorTorque,
											float lowerLimit, float upperLimit)
	{
		b2Body* bodyA = c1->physicsBody();
		b2Body* bodyB = c2->physicsBody();

		b2RevoluteJointDef *jointDef = new b2RevoluteJointDef();
		jointDef->bodyA = bodyA;
		jointDef->bodyB = bodyB;

		QPointF localPosA = c1->mapFromScene(scenePos) * c1->scale();
		QPointF localPosB = c2->mapFromScene(scenePos) * c2->scale();

		jointDef->localAnchorA.Set(localPosA.x()/defaultPhysicsScale,
								  localPosA.y()/defaultPhysicsScale);
		jointDef->localAnchorB.Set(localPosB.x()/defaultPhysicsScale,
								  localPosB.y()/defaultPhysicsScale);

		jointDef->enableLimit = enableLimits;
		jointDef->upperAngle = upperLimit;
		jointDef->lowerAngle = lowerLimit;

		jointDef->enableMotor = enableMotor;
		jointDef->motorSpeed = motorSpeed;
		jointDef->maxMotorTorque = motorTorque;

		b2Joint* joint = createJoint(*jointDef);

		PhysicsJoint *physicsJoint = new PhysicsJoint(this);
		physicsJoint->componentA = c1;
		physicsJoint->componentB = c2;
		physicsJoint->relPosA = c1->mapFromScene(scenePos);
		physicsJoint->relPosB = c2->mapFromScene(scenePos);
		physicsJoint->_jointDef = jointDef;
		physicsJoint->_joint = joint;
		physicsJoint->_box2dJointType = e_revoluteJoint;
		joint->SetUserData(static_cast<void*>(physicsJoint));

		c1->addJoint(physicsJoint);
		c2->addJoint(physicsJoint);

		_jointList.push_back(physicsJoint);

		return physicsJoint;
	}

	PhysicsJoint* PhysicsManager::createPrismaticJoint(Component *c1, Component *c2,
											   QPointF startPos, QPointF endPos,
											   bool enableMotoe, bool enableLimits,
											   float motorSpeed, float motorForce,
											   float lowerLimit, float upperLimit)
	{
		b2PrismaticJointDef* jointDef = new b2PrismaticJointDef;
		jointDef->bodyA = c1->physicsBody();
		jointDef->bodyB = c1->physicsBody();

		QPointF direction = endPos - startPos;

		return NULL;
	}

	bool PhysicsManager::updateJoint(PhysicsJoint *joint)
	{
		QPointF localPosA = joint->relPosA * joint->componentA->scale();
		QPointF localPosB = joint->relPosB * joint->componentB->scale();

		switch (joint->_box2dJointType)
		{
		case e_revoluteJoint :
		{
			b2RevoluteJointDef* revoluteJointDef = static_cast<b2RevoluteJointDef*>(joint->_jointDef);

			revoluteJointDef->localAnchorA.Set
					(localPosA.x()/defaultPhysicsScale, localPosA.y()/defaultPhysicsScale);
			revoluteJointDef->localAnchorB.Set
					(localPosB.x()/defaultPhysicsScale, localPosB.y()/defaultPhysicsScale);

			b2RevoluteJoint* prevRevoluteJoint = static_cast<b2RevoluteJoint *>(joint->_joint);
			b2RevoluteJoint* newRevoluteJoint = static_cast<b2RevoluteJoint*>(createJoint(*revoluteJointDef));

			// Copy values before deleting previous joint
			newRevoluteJoint->SetMaxMotorTorque(prevRevoluteJoint->GetMaxMotorTorque());
			newRevoluteJoint->SetMotorSpeed(prevRevoluteJoint->GetMotorSpeed());
			newRevoluteJoint->EnableMotor(prevRevoluteJoint->IsMotorEnabled());
			newRevoluteJoint->SetLimits(prevRevoluteJoint->GetLowerLimit(),
										prevRevoluteJoint->GetUpperLimit());
			newRevoluteJoint->SetUserData(prevRevoluteJoint->GetUserData());

			_b2World->DestroyJoint(joint->_joint);
			joint->_joint = newRevoluteJoint;
			return true;
		}
		}

		return false;
	}

	bool PhysicsManager::updateJoint(PhysicsJoint *joint, QPointF newScenePos)
	{
		joint->relPosA = joint->componentA->mapFromScene(newScenePos);
		joint->relPosB = joint->componentB->mapFromScene(newScenePos);
		return updateJoint(joint);
	}

	bool PhysicsManager::deleteJoint(PhysicsJoint *joint)
	{
		if (_jointList.contains(joint))
			_jointList.removeOne(joint);
		joint->_physicsManager = this;
		delete joint;
		return true;
	}

	bool PhysicsManager::deleteJoint(b2Joint *joint)
	{
		if (joint == 0)	return false;

		_b2World->DestroyJoint(joint);
		return true;
	}

	/*
	b2Joint* PhysicsManager::createWheelJoint(Component* c1, Component* c2,
											  b2WheelJointDef& def)
	{
		return NULL;
	}

	b2Joint* PhysicsManager::createGearJoint(Component* j1, Component* j2,
											 b2GearJointDef& def)
	{
		return NULL;
	}

	b2Joint* PhysicsManager::createGearJoint(Component* j1, Component* j2,
											 float gearRatio)
	{
		return NULL;
	}

	b2Joint* PhysicsManager::createWeldJOint(Component* c1, Component* c2,
											 b2WeldJointDef* def)
	{
		return NULL;
	}
	*/

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

		_isRunning = true;
		if (debugView) debugView->clear();
	}

	void PhysicsManager::step()
	{
		if (_b2World == NULL) return;

		emit physicsStepStart();
		_b2World->Step(_settings.timeStep, _settings.velocityIterations,
					   _settings.positionIterations);
		if (_enableDebugView)
		{
			debugView->clear();
			_b2World->DrawDebugData();
		}
		emit physicsStepComplete();
	}

	void PhysicsManager::setEnableGravity(bool enable)
	{
		if (enable)
			_b2World->SetGravity
					(b2Vec2(_settings.gravity.x(), _settings.gravity.y()));
		else
			_b2World->SetGravity(b2Vec2(0,0));
	}

	void PhysicsManager::setEnableMotor(bool enable)
	{
		qDebug() << "All motors " << enable;
		foreach (PhysicsJoint* physicsJoint, _jointList)
		{
			qDebug() << "Inside motor";
			switch (physicsJoint->_box2dJointType)
			{
				case e_revoluteJoint :
				{
					b2RevoluteJoint* pinJoint =	static_cast<b2RevoluteJoint*>(physicsJoint->_joint);
					b2RevoluteJointDef* pinJointDef = static_cast<b2RevoluteJointDef*>(physicsJoint->_jointDef);
					enable ? pinJoint->EnableMotor(pinJointDef->enableMotor) : pinJoint->EnableMotor(false);
					break;
				}
				case e_prismaticJoint :
				{
					b2PrismaticJoint* sliderJoint = static_cast<b2PrismaticJoint*>(physicsJoint->_joint);
					b2PrismaticJointDef* sliderJointDef = static_cast<b2PrismaticJointDef*>(physicsJoint->_jointDef);
					enable ? sliderJoint->EnableMotor(sliderJointDef->enableMotor) : sliderJoint->EnableMotor(false);
					break;
				}
			}

			if (!enable)
			{
				b2Body* bodyA = physicsJoint->_joint->GetBodyA();
				b2Body* bodyB = physicsJoint->_joint->GetBodyB();

				bodyA->SetLinearVelocity(b2Vec2(0,0));
				bodyB->SetLinearVelocity(b2Vec2(0,0));

				bodyA->SetAngularVelocity(0);
				bodyB->SetAngularVelocity(0);
			}
		}

		QLOG_INFO() << "All joint motor" << enable;
	}

	void PhysicsManager::setGlobalCollision
			(QList<Component*> components, bool enableCollision)
	{
		foreach(Component* component, components)
		{
			b2Body* body = component->physicsBody();
			body->playgoCollisionEnabled = enableCollision;
		}
	}

	void PhysicsManager::updateComponentPosition(QList<Component *> &components)
	{
		foreach(Component* component, components)
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
		foreach(Component* component, components)
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

			b2Body* body = component->physicsBody();
			Point2D pos = component->pos();
			float rotation = component->rotation() * _PI_/180.0f;
			b2Vec2 boxPos = b2Vec2(pos.x()/defaultPhysicsScale, pos.y()/defaultPhysicsScale);
			b2Vec2 currentPos = body->GetPosition();
//			if (true)	// NO velocity update
//			{
//				b2Vec2 velocity = body->GetLinearVelocity();
//				b2Vec2 newVelocity = b2Vec2(velocity.x+boxPos.x-currentPos.x,
//											velocity.y+boxPos.y-currentPos.y);
//				body->SetLinearVelocity(newVelocity);
//			} else
//				body->SetLinearVelocity(b2Vec2(0,0));	// pause the object which is being moved.
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
		if(component->physicsBody() == NULL)
		{
			b2BodyDef  def;
			def.type = b2_dynamicBody;
			Point2D pos = component->pos();
			def.position = b2Vec2(pos.x()/defaultPhysicsScale, pos.y()/defaultPhysicsScale);
			def.angle = component->rotation() * (_PI_ / 180.0f);	// Angle in radians
			b2Body* newBody =_b2World->CreateBody(&def);
			newBody->SetUserData(static_cast<void*>(component));
			component->setPhysicsBody(newBody);

		}
	}
}
