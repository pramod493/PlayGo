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
	PhysicsManager::PhysicsManager(PhysicsSettings *settings, Page *parentPage) : QObject(parentPage)
	{
		defaultPhysicsScale = getPhysicsScale();

		_settings.timeStep = settings->timeStep;
		_settings.velocityIterations = settings->velocityIterations;
		_settings.positionIterations = settings->positionIterations;
		_settings.gravity = settings->gravity;
		_settings.enableSleep = settings->enableSleep;

		b2Vec2 gravity(_settings.gravity.x(), _settings.gravity.y());
		_b2World = new b2World(gravity);
		_b2World->SetAllowSleeping(_settings.enableSleep);


		debugView = new BoxDebugScene(defaultPhysicsScale);

		debugView->SetFlags(b2Draw::e_shapeBit | b2Draw::e_pairBit | b2Draw::e_jointBit | b2Draw::e_centerOfMassBit);// |/* b2Draw::e_aabbBit |*/  b2Draw::e_centerOfMassBit);

		_b2World->SetDebugDraw(debugView);

		timer = NULL;

		_internalLock = false;
		_isRunning = false;
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

		return physicsJoint;
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

			_b2World->DestroyJoint(joint->_joint);

			joint->_joint = createJoint(*revoluteJointDef);
			return true;
		}
		}

		return false;
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
	}

	void PhysicsManager::start(int timerStepIn_msecs)
	{
		if (timer != NULL)
		{
			delete timer;
		}
		timer = new QTimer(this);
		connect(timer, SIGNAL(timeout()),
				this, SLOT(step()));
		timer->start(timerStepIn_msecs);
		_isRunning = true;
	}

	void PhysicsManager::step()
	{
		if (_b2World == NULL) return;

		emit physicsStepStart();
		_b2World->Step(_settings.timeStep, _settings.velocityIterations,
					   _settings.positionIterations);
		debugView->clear();
		_b2World->DrawDebugData();
		emit physicsStepComplete();
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
