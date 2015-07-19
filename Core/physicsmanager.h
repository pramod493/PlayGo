#pragma once
#include <QObject>
#include <Box2D/Box2D.h>
#include <Box2D/Dynamics/b2Body.h>
#include <QTimer>
#include <QHash>
#include "physicsbody.h"
#include "physicsjoint.h"
#include <QTimer>

class BoxDebugScene;
namespace CDI
{
	class Page;
	class Component;
	// Use the b2BodyDef for setting the value of physics components
	/**
	 * @brief physicsBodyDef stores the definition of property of physics body
	 * including
	 * 1. Position & angle
	 * 2. Initial linear and angular velocity
	 * 3. Damping
	 * 4. Gavity scale and other props
	 */
	typedef struct b2BodyDef physicsBodyDef;

	class PhysicsSettings {
	public:
		float timeStep;
		float velocityIterations;
		float positionIterations;
		Point2D gravity;
		bool enableSleep;

		PhysicsSettings()
		{
			timeStep=1.0f/60.0f;
			velocityIterations = 6;
			positionIterations = 4;
			gravity = Point2D(0.0f,10.0f);
			enableSleep = true;
		}

		QDataStream& serialize(QDataStream& stream) const
		{
			stream << timeStep;
			stream << velocityIterations;
			stream << positionIterations;
			stream << gravity;
			stream << enableSleep;
			return stream;
		}
		// NOTE - Why does the return type have to be QDataStream. Can't it be void?
		QDataStream& deserialize(QDataStream& stream)
		{
			stream >> timeStep;
			stream >> velocityIterations;
			stream >> positionIterations;
			stream >> gravity;
			stream >> enableSleep;
			return stream;
		}
	};

	class PhysicsManager : public QObject
	{
		Q_OBJECT

		float defaultPhysicsScale;

	protected:
		QHash<QUuid, b2Body*> _box2DBodies;
		QHash<QUuid, PhysicsBody*> _physicsBodies;

		PhysicsSettings _settings;

		QTimer* timer;

		bool _internalLock;

		bool _isRunning;

		BoxDebugScene* debugView;

		b2World* _b2World;

	public:
		explicit PhysicsManager(PhysicsSettings* settings, Page *parentPage);

		~PhysicsManager();

		// Factory functions for creating joints. Keeping them here saves from
		// creating multiple derived classes for joints
		virtual b2Body *createBody(const b2BodyDef& def);

		virtual PhysicsJoint* createPinJoint(Component *c1, Component *c2, QPointF scenePos,
										bool enableMotor, bool enableLimits,
										float motorSpeed, float motorTorque,
										float lowerLimit, float upperLimit);

		virtual bool updateJoint(PhysicsJoint* joint);

		/*virtual b2Joint* createWheelJoint(Component* c1, Component* c2,
											   b2WheelJointDef& def);
		virtual b2Joint* createGearJoint(Component* j1, Component* j2,
											  b2GearJointDef& def);
		virtual b2Joint* createGearJoint(Component* j1, Component* j2,
											  float gearRatio);
		virtual b2Joint* createWeldJOint(Component* c1, Component* c2,
											  b2WeldJointDef* def);
											  */
		/**
		 * @brief updateSettings updates the settings of b2World object and applies to them
		 * @param newSettings
		 */
		void updateSettings(PhysicsSettings* newSettings);

	protected:
		virtual b2Joint* createJoint(b2JointDef& jointDef);

	signals:
		void physicsStepStart();

		void physicsStepComplete();

		void physicsPartUpdate();

	public slots:
		void pause();

		void start(int timerStepIn_msecs);

		void step();

		void updateComponentPosition(QList<Component*>& components);

		void updateComponentPosition(Component* component);

		void updateFromComponentPosition(QList<Component*>& components);

		void updateFromComponentPosition(Component* component);

		void onItemAdd(QGraphicsItem* item);
	};
}

/* Random code snippets
 *
 * =============== b2World* physicsWorld;
 *
 *
 * Initializing the physics engine as well as testing out the simulation aspect
 * See http://www.box2d.org/manual.html for tutorial for more
	if (false) {
		b2Vec2 gravity(0.0, -2.0);
		//        bool doSleep = true;
		physicsWorld = new b2World(gravity);

		// Create ground
		b2BodyDef groundBodyDef;    // Create as object so that it automatically gets deleted at the end
		groundBodyDef.position.Set(0.0,-10.0);

		b2Body* groundBody = physicsWorld->CreateBody(&groundBodyDef);
		b2PolygonShape groundBox;
		groundBox.SetAsBox(50.0,10.0);
		groundBody->CreateFixture(&groundBox, 0.0);

		QTimer* timer = new QTimer();
		connect(timer, SIGNAL(timeout()),
				this, SLOT(PhysicsStep()));
		timer->start(10);
	}
*
*
* 		 Physics engine test code
			{
				b2BodyDef bodyDef;
				bodyDef.type = b2_dynamicBody;
				bodyDef.position.Set(0.0,0.0);
				bodyDef.angle = 25;
				current_stroke->physicsBody = physicsWorld->CreateBody(&bodyDef);
				b2ChainShape polygon;
				b2Vec2* vec = new b2Vec2[current_stroke->parentStroke->points.size()];
				int vertexCount =0;
				for (int i=0; i < current_stroke->parentStroke->points.size();)
				{
					Point2D* pt = current_stroke->parentStroke->points[i];
					vec[i] = b2Vec2(pt->x*current_stroke->physicsDivider,pt->y*current_stroke->physicsDivider);
					vertexCount++;
					i++;
				}
				polygon.CreateChain(vec, vertexCount);
				b2FixtureDef fixtureDef;
				fixtureDef.shape = &polygon;
				fixtureDef.density = 0.4f;
				fixtureDef.friction = 0.25f;
				fixtureDef.restitution = 0.75f;
				current_stroke->physicsBody->CreateFixture(&fixtureDef);

			}
	* Step physics engine
	*
	*
	*
	*
	*
	* and update all the components
	* Disable the whole physics engine runtime for now
			// Set up simulation settings
			float timeStep = 1.0/60.0;
			int velocityIterations = 6;
			int positionIterations = 4;

			physicsWorld->Step(timeStep, velocityIterations, positionIterations);
			GraphicsPathItem* item = NULL;

			foreach(item, freeStrokes)
			{
				if (item->physicsBody!= NULL)
				{
					b2Body* body = item->physicsBody;
					b2Vec2 position = body->GetPosition();
					float angle = body->GetAngle();
					item->setPos(position.x*item->physicsMultiplier,position.y*item->physicsMultiplier);
					item->setRotation(angle);
					update(item->boundingRect());
					qDebug() << angle << position.x << position.y;
				}
			}
			if (item != NULL)
			if (item->physicsBody!= NULL)
			{
				qDebug() << item->physicsBody->GetAngle();
			}
*/
