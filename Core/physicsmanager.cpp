#include <QList>
#include "physicsmanager.h"
#include "page.h"
#include <QDebug>
#include "QsLog.h"
#include "component.h"

namespace CDI
{
	PhysicsManager::PhysicsManager(PhysicsSettings *settings, Page *parentPage) : QObject(parentPage)
	{
		defaultPhysicsScale = 100.0f;

		_settings.timeStep = settings->timeStep;
		_settings.velocityIterations = settings->velocityIterations;
		_settings.positionIterations = settings->positionIterations;
		_settings.gravity = settings->gravity;
		_settings.enableSleep = settings->enableSleep;

		b2Vec2 gravity(_settings.gravity.x(), _settings.gravity.y());
		_b2World = new b2World(gravity);
		_b2World->SetAllowSleeping(_settings.enableSleep);
		timer = NULL;

		_internalLock = false;
		_isRunning = false;

		connect(this, SIGNAL(physicsStepComplete()),
				this, SLOT(updateComponentPosition()));
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

	PhysicsJoint* PhysicsManager::createJoint()
	{
		QLOG_INFO() << "@PhysicsManager::createJoint() not implemented";
		// TODO - Implementation of joint creation
		return NULL;
	}

	PhysicsJoint* PhysicsManager::createRevoluteJoint(PhysicsBody* c1, PhysicsBody* c2,
											  b2RevoluteJointDef *def)
	{
		return NULL;
	}

	PhysicsJoint* PhysicsManager::createWheelJoint(PhysicsBody* c1, PhysicsBody* c2,
										   b2WheelJointDef* def)
	{
		return NULL;
	}

	PhysicsJoint* PhysicsManager::createGearJoint(PhysicsJoint* j1, PhysicsJoint* j2,
										  b2GearJointDef* def)
	{
		return NULL;
	}

	PhysicsJoint* PhysicsManager::createGearJoint(PhysicsJoint* j1, PhysicsJoint* j2,
										  float gearRatio)
	{
		return NULL;
	}

	PhysicsJoint* PhysicsManager::createWeldJOint(PhysicsBody* c1, PhysicsBody* c2,
										  b2WeldJointDef* def)
	{
		return NULL;
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
		if (timer!= NULL)
			timer->stop();
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
		if (_b2World != NULL)
		{
			_b2World->Step(_settings.timeStep, _settings.velocityIterations,
								_settings.positionIterations);
//			QLOG_INFO() << "Stepping through physics";
			b2Body* b = _b2World->GetBodyList();
			int n = _b2World->GetBodyCount();
			if(b != NULL)
			QLOG_INFO() << b->GetPosition().x << b->GetPosition().y;
		}
		emit physicsStepComplete();
	}

	void PhysicsManager::updateComponentPosition()
	{
		QList<PhysicsBody*> items = _physicsBodies.values();
		foreach(PhysicsBody* item, items)
		{
			b2Body* body = item->getBox2DBody();
			QTransform t= QTransform();
			t.rotate(body->GetAngle());
			t.translate(body->GetPosition().x, body->GetPosition().y);
			item->setTransform(t);
		}

		emit physicsPartUpdate();
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

	void PhysicsManager::updateFromComponentPosition(Component *component)
	{
		if (component->physicsBody() && !_internalLock)
		{
			b2Body* body = component->physicsBody();
			Point2D pos = component->pos();
			float rotation = component->rotation() * _PI_/180.0f;
			QLOG_INFO() << "Position" << pos << "Rotation" << rotation;
			b2Vec2 boxPos = b2Vec2(pos.x()/defaultPhysicsScale, pos.y()/defaultPhysicsScale);
			b2Vec2 currentPos = body->GetPosition();
			if (_isRunning)
			{
				b2Vec2 velocity = body->GetLinearVelocity();
				b2Vec2 newVelocity = b2Vec2(velocity.x+boxPos.x-currentPos.x,
											velocity.y+boxPos.y-currentPos.y);
				body->SetLinearVelocity(newVelocity);
				QLOG_INFO() << "Updated velocity" << velocity.x << velocity.y << newVelocity.x << newVelocity.y;
			}
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
			component->setPhysicsBody(newBody);

			QRectF boundingBox = component->boundingRect();
			{
				b2PolygonShape dummy;
				b2Vec2 center = b2Vec2(boundingBox.center().x()/defaultPhysicsScale,
									   boundingBox.center().y()/defaultPhysicsScale);
				dummy.SetAsBox(boundingBox.width()*0.5f/defaultPhysicsScale,
							   boundingBox.height()*0.5f/defaultPhysicsScale, center, 0);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &dummy;
				fixtureDef.density = 1.0f;
			}
		}
	}
}
