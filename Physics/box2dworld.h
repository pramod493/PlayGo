#pragma once
#include <QObject>
#include <QHash>
#include <QDebug>
#include <QTimer>
#include <QTimerEvent>
#include <Box2D/Box2D.h>
#include <Box2D/Dynamics/b2World.h> // Are all the extra necessary

namespace CDI
{
class Box2DWorld : public QObject
{
	Q_OBJECT
protected:
	QHash<b2Joint*, b2Body*> p_jointManager;

	b2World *p_b2World;

	int p_timerId;

	int p_isRunning;

	float32 p_timeStep;

	int32 p_velocityIterations;

	int32 p_positionIterations;

public:
	explicit Box2DWorld(const b2Vec2 &gravity = b2Vec2(0,0),
						QObject *parent = 0);

	~Box2DWorld() {
		delete p_b2World;
	}

	//// Simulation options
	void Start() {
		p_timerId = startTimer(p_timeStep * 1000);
		p_isRunning = 1;
	}

	void Pause() {
		if (p_timerId > 0) killTimer(p_timerId);
		p_timerId = 0;
		p_isRunning = 0;
	}

	void Reset() {
		qDebug() << "Reset the simulation";
	}

	//// Object management in the world
	b2Body* CreateBody(const b2BodyDef *qb2BodyDef);

	void DestroyBody(b2Body* body)
	{
		p_b2World->DestroyBody(body);
	}

	void CreateJoint();

	void DestroyJoint(b2Joint* joint)
	{
		B2_NOT_USED(joint);
	}

	//// Simulation settings tampering
	void SetGravity();

	void GetGravity();

	void Lock();

	bool isLocked();

	//// Individual object behavior configuration
	void SetAutoClearForces();

	void GetAutoClearForces();

	void ClearForces();

	//// World flags - Not of the normal types
	void SetAllowSleeping();

	void GetAllowSleeping();

	void SetWarmStarting();

	void GetWarmStarting();

	void SetContinuousPhysics();

	void GetContinuousPhysics();

	void SetSubStepping();

	void GetSubstepping();

	void GetContactManager();

	void GetProfile();

	void SetDestructionListener();

	void SetContactListener();

	void QueryAABB();

	void Raycast();

	int GetProxyCount();

	int GetBodyCount();

	int GetJointCount();

	int GetContactCount();

	void SetTimeStep(float tStep);

	void SetVelocityIterations(int velIters);

	void SetPositionIterations(int posIters);

	bool isRunning();

	virtual void Step();

signals:

public slots:
	void simulate(bool flag);

protected:
	void timerEvent(QTimerEvent *event);
};
}
