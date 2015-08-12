#pragma once
#include <QObject>
#include <Box2D/Box2D.h>
#include <Box2D/Dynamics/b2Body.h>
#include <QTimer>
#include <QHash>
#include "physicsbody.h"
#include "physicsjoint.h"
#include "material.h"
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
		const unsigned int physicsPaused 		= 0;
		const unsigned int noGravity			= MASK_BIT0;
		const unsigned int noCollision			= MASK_BIT1;
		const unsigned int noMotor				= MASK_BIT2;

	enum cdEntityCategory {
		ENTITY_GROUND	= 0x0001,
		ENTITY_LAYER_01	= 0x0002,
		ENTITY_LAYER_02	= 0x0004,
		ENTITY_LAYER_03	= 0x0008,
		ENTITY_LAYER_04	= 0x0010,
		ENTITY_LAYER_05	= 0x0020,
		ENTITY_LAYER_06	= 0x0040,
		ENTITY_LAYER_07	= 0x0080
	};

	enum cdMaskCategory {
		MASK_GROUND		= 0x0001,
		MASK_LAYER_01	= 0x0002,
		MASK_LAYER_02	= 0x0004,
		MASK_LAYER_03	= 0x0008,
		MASK_LAYER_04	= 0x0010,
		MASK_LAYER_05	= 0x0020,
		MASK_LAYER_06	= 0x0040,
		MASK_LAYER_07	= 0x0080
	};

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

		PhysicsSettings(const PhysicsSettings& settings)
		{
			timeStep			= settings.timeStep;
			velocityIterations	= settings.velocityIterations;
			positionIterations	= settings.positionIterations;
			gravity				= settings.gravity;
			enableSleep			= settings.enableSleep;
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

	protected:
		float defaultPhysicsScale;

		QHash<QUuid, b2Body*> _box2DBodies;
		QHash<QUuid, PhysicsBody*> _physicsBodies;

		QList<PhysicsJoint*> _jointList;

		PhysicsSettings _settings;

		QTimer* timer;

		bool _internalLock;

		bool _isRunning;

		b2World* _b2World;
protected:
		bool _enableDebugView;
	public:
		BoxDebugScene* debugView;

	public:
		PhysicsManager(PhysicsSettings* settings, Page *parentPage);

		explicit PhysicsManager(QObject *parent = 0);

		PhysicsManager(const PhysicsManager& physicsManager);

		~PhysicsManager();

		// Factory functions for creating joints. Keeping them here saves from
		// creating multiple derived classes for joints
		virtual b2Body *createBody(const b2BodyDef& def);

		virtual void destroyBody(b2Body* body);

		virtual void destroyBody(Component* component);

		virtual PhysicsJoint* createPinJoint(Component *c1, Component *c2, QPointF scenePos,
										bool enableMotor, bool enableLimits,
										float motorSpeed, float motorTorque,
										float lowerLimit, float upperLimit);

		virtual PhysicsJoint* createPrismaticJoint(Component *c1, Component *c2,
												   QPointF startPos, QPointF endPos,
												   bool enableMotoe, bool enableLimits,
												   float motorSpeed, float motorForce,
												   float lowerLimit, float upperLimit);

		virtual bool updateJoint(PhysicsJoint* joint);

		virtual bool updateJoint(PhysicsJoint* joint, QPointF newScenePos);

		virtual bool deleteJoint(PhysicsJoint* joint);

		virtual bool deleteJoint(b2Joint* joint);

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

		void setEnableDebugView(bool enableDebugView);

		bool enableDebugView() const;

		virtual b2Joint* createJoint(b2JointDef& jointDef);

	protected:
		virtual void init();

	signals:
		void physicsStepStart();

		void physicsStepComplete();

		void physicsPartUpdate();

	public slots:
		void pause();

		void start(int timerStepIn_msecs);

		void step();

		// Settings update
		void setEnableGravity(bool enable);

		void setEnableMotor(bool value);

		void setGlobalCollision(QList<Component*> components, bool enableCollision);	// use playgoCollision parameter

		// Manage component state
		void updateComponentPosition(QList<Component*>& components);

		void updateComponentPosition(Component* component);

		void updateFromComponentPosition(QList<Component*>& components);

		void updateFromComponentPosition(Component* component);

		void onItemAdd(QGraphicsItem* item);
	};
}

Q_DECLARE_METATYPE(CDI::Material)
Q_DECLARE_METATYPE(CDI::PhysicsSettings)
Q_DECLARE_METATYPE(CDI::PhysicsManager)

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
