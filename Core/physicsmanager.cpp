#include <QList>
#include "physicsmanager.h"
#include "page.h"
#include <QDebug>
#include "QsLog.h"

namespace CDI
{
	PhysicsManager::PhysicsManager(PhysicsSettings *settings, Page *parentPage) : QObject(parentPage)
	{
		_settings.timeStep = settings->timeStep;
		_settings.velocityIterations = settings->velocityIterations;
		_settings.positionIterations = settings->positionIterations;
		_settings.gravity = settings->gravity;
		_settings.enableSleep = settings->enableSleep;

		b2Vec2 gravity(_settings.gravity.x(), _settings.gravity.y());
		_b2World = new b2World(gravity);
		_b2World->SetAllowSleeping(_settings.enableSleep);
		timer = NULL;

		connect(this, SIGNAL(physicsStepComplete()),
				this, SLOT(updateComponentPosition()));
	}

	PhysicsManager::~PhysicsManager()
	{
		if (_b2World!= NULL) delete _b2World;
	}

	PhysicsBody* PhysicsManager::createBody(Component *component,const physicsBodyDef *def)
	{
		if (_b2World!= NULL)
		{
			b2Body* body = _b2World->CreateBody(def);
			PhysicsBody* physicsBody = new PhysicsBody(component, body);
			return physicsBody;
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
	}

	void PhysicsManager::step()
	{
		if (_b2World != NULL)
			_b2World->Step(_settings.timeStep, _settings.velocityIterations,
								_settings.positionIterations);

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
}
