#pragma once
#include <QObject>
#include <Box2D/Box2D.h>
#include <Box2D/Dynamics/b2Body.h>
#include <QTimer>
#include <QHash>
#include "physicsbody.h"
#include "physicsjoint.h"
#include "material.h"
#include "cdcontactlistener.h"
#include <QTimer>

#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

using namespace boost;

class BoxDebugScene;

struct GraphProperty
{
	QString name;
	QUuid graphID;
};

namespace CDI
{
	class Page;
	class Component;
	class cdJoint;

	using cdComponentGraph
	= adjacency_list<vecS, vecS, undirectedS, Component*/*, cdJoint*, GraphProperty*/>;
	using componentVertex = graph_traits<cdComponentGraph>::vertex_descriptor;
	using componentEdge = graph_traits<cdComponentGraph>::edge_descriptor;
	//typedef std::pair<Component*, Component*> Edge;
	// Use the b2BodyDef for setting the value of physics components
	/**
	 * @brief b2BodyDef stores the definition of property of physics body
	 * including
	 * 1. Position & angle
	 * 2. Initial linear and angular velocity
	 * 3. Damping
	 * 4. Gavity scale and other props
	 */
	const unsigned int physicsPaused 		= 0;
	const unsigned int noGravity			= MASK_BIT0;
	const unsigned int noCollision			= MASK_BIT1;
	const unsigned int noMotor				= MASK_BIT2;

	class PhysicsSettings {
	public:
		float timeStep;
		float velocityIterations;
		float positionIterations;
		Point2D gravity;
		bool enableSleep;
		bool enableGravity;
		bool enableMotor;

		PhysicsSettings()
		{
			timeStep=1.0f/60.0f;
			velocityIterations = 6;
			positionIterations = 4;
			gravity = Point2D(0.0f,10.0f);
			enableSleep = true;
			enableGravity = false;
			enableMotor = false;
		}

		PhysicsSettings(const PhysicsSettings& settings)
		{
			timeStep						= settings.timeStep;
			velocityIterations	= settings.velocityIterations;
			positionIterations	= settings.positionIterations;
			gravity							= settings.gravity;
			enableSleep					= settings.enableSleep;
			enableGravity 			= settings.enableGravity;
			enableMotor					= settings.enableMotor;
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
		float defaultPhysicsScale;	/**< Stores the global physics scale to apply to the system */

		// Manage items and relationships
		QHash<QUuid, b2Body*> _box2DBodies;
		QList<cdJoint*> _jointList;

		PhysicsSettings _settings;	/**< Settings for physics engigne */

		QTimer* timer;				/**< Timer to call the step function */

		bool _internalLock;			/**< Prevents looping while updating the component positions */

		bool _isRunning;			/**< True if the box2d is running, false otherwise */

		bool _quickpause;			/**< Pauses the physics engine without stopping the timer*/

		b2World* _b2World;			/**< b2World pointer */

		cdContactListener* _contactListener; /**< Implements collision listener */

		bool _enableDebugView;		/**< Determines whether debug view will be updated or not */
	public:
		BoxDebugScene* debugView;	/**< Renders the debug information */

		cdComponentGraph connectionsGraph;

	public:
		/**
		 * @brief Instantiate PhysicsManager object with given settings and
		 * parent Page
		 * @param settings Physics settings at start
		 * @param parentPage Parent Page. Deleted with page
		 */
		PhysicsManager(PhysicsSettings& settings, QObject *parent);

		/**
		 * @brief For Q_METATYPE
		 * @param parent parent QObject
		 */
		explicit PhysicsManager(QObject *parent = nullptr);

		/**
		 * @brief Instantiate PhysicsManager object with settings from given instance
		 * @param physicsManager Existing object
		 */
		PhysicsManager(const PhysicsManager& physicsManager);

		/**
		 * @brief ~PhysicsManager destroys the box2DWorld along with
		 * all the bodies in it
		 */
		virtual ~PhysicsManager();

		/**
		 * @brief Destroys the b2Body associated with given Component
		 * @param component Component whose b2Body is to be deleted
		 */
		virtual void destroyBody(Component* component);

		/**
		 * @brief createPinJoint creates a pin joint between two components
		 * @param c1 Component 1
		 * @param c2 Component 2
		 * @param scenePos Position of pin joint in global coords
		 * @param enableMotor Is motor enabled?
		 * @param enableLimits Is motion restricted
		 * @param motorSpeed Motor speed desired
		 * @param motorTorque Maximum torque generated by the motor
		 * @param lowerLimit Lower limit of motion
		 * @param upperLimit Upper limit of motion (angle in radians)
		 * @return PhysicsJoint created
		 */
		virtual cdPinJoint* createPinJoint
		(Component *c1, Component *c2, QPointF scenePos,
		 bool enableMotor, bool enableLimits,
		 float motorSpeed, float motorTorque,
		 float lowerLimit, float upperLimit);

		/**
		 * @brief createPrismaticJoint creates a prismatic joint between
		 * Component c1 and c2 where the joint is limited to startPos and endPos.
		 * @param c1 bodyA
		 * @param c2 bodyB
		 * @param startPos		Start of constraint
		 * @param endPos		End of constraint
		 * @param enableMotor	Turn motor on
		 * @param enableLimits	Limit motion (true by default)
		 * @param motorSpeed	Desired motor speed
		 * @param motorForce	Maximum torque allowed
		 * @param lowerLimit	Min limit of joint
		 * @param upperLimit	Max limit of joint
		 * @return new cdSliderJoint
		 */
		virtual cdSliderJoint* createPrismaticJoint
		(Component *c1, Component *c2,
		 QPointF startPos, QPointF endPos,
		 bool enableMotor, bool enableLimits,
		 float motorSpeed, float motorForce/*,
		 float lowerLimit, float upperLimit*/);

		/**
		 * @brief Updates the joint with updated definitions. Recreates b2Joint if needed
		 * @param joint Joint to update
		 * @return True if successfull, false otherwise
		 */
		virtual bool updateJoint(cdJoint* physicsJoint);

		virtual bool moveJoint(cdJoint* joint, QPointF newScenePos);

		virtual bool deleteJoint(cdJoint* joint);

		/**
		 * @brief updateSettings updates the settings of b2World object and applies to them
		 * @param newSettings
		 */
		void updateSettings(PhysicsSettings* newSettings);

		/**
		 * @brief setEnableDebugView controls whether the debug information is displayed or not
		 * @param enableDebugView
		 */
		void setEnableDebugView(bool enableDebugView);

		bool enableDebugView() const;

		virtual b2Joint* createJoint(b2JointDef& jointDef);

	protected:
		virtual void init();

		/**
		 * @brief Factory functions for creating joints. Keeping them here saves
		 * from creating multiple derived classes for joints
		 * @param def b2BodyDef
		 * @return b2Body reference
		 */
		virtual b2Body *createBody(const b2BodyDef& def);

		/**
		 * @brief Destroys the b2Body
		 * @param body b2Body to delete
		 */
		virtual void destroyBody(b2Body* body);

		/**
		 * @brief deleteJoint deletes b2Joint from box 2d
		 * @param joint
		 * @return
		 */
		virtual bool deleteJoint(b2Joint* joint);

	signals:
		void physicsStepStart();

		void physicsStepComplete();

		void physicsPartUpdate();

	public slots:
		/*----------- Simulation options ---------------*/
		void pause();

		void start(int timerStepIn_msecs);

		void step();

		void quickPause(bool enable);

		/*----------- Physics settings -----------------*/
		void setEnableGravity(bool enable);
		bool isGravityEnabled() const;

		/*----------- Configure components and joints -*/
		/**
		 * @brief setEnableMotor updates the motor component of all joints
		 * @param value
		 */
		void setEnableMotor(bool value);

		/**
		 * @brief isMotorEnabled returns the global override of motors/actuators
		 * @return
		 */
		bool isMotorEnabled() const;

		/**
		 * @brief setGlobalCollision toggles the global override to enable/disable collision of components
		 * @param components
		 * @param enableCollision
		 */
		void setGlobalCollision(QList<Component*> components, bool enableCollision);	// use playgoCollision parameter

		/**
		 * @brief updateComponentPosition updates component position from box
		 * @param components
		 */
		void updateComponentPosition(QList<Component*>& components);

		/**
		 * @brief updateComponentPosition updates component position from box
		 * @param component
		 */
		void updateComponentPosition(Component* component);

		/**
		 * @brief updateFromComponentPosition updates the b2Body tranform
		 * based on the Component transform for list of components
		 * @param components
		 */
		void updateFromComponentPosition(QList<Component*>& components);

		/**
		 * @brief updateFromComponentPosition updates the b2Body transform
		 * based on component transform
		 * @param component
		 */
		void updateFromComponentPosition(Component* component);

		/**
		 * @brief onItemAdd is called when a new GraphicsItem is added to scene
		 * It adds a b2Body if the item is component
		 * @param item
		 */
		void onItemAdd(QGraphicsItem* item);

		void addPhysicsBody(Component* component);
	};
}

Q_DECLARE_METATYPE(CDI::Material)
Q_DECLARE_METATYPE(CDI::PhysicsSettings)
//Q_DECLARE_METATYPE(CDI::PhysicsManager)

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
					QLOG_INFO() << angle << position.x << position.y;
				}
			}
			if (item != NULL)
			if (item->physicsBody!= NULL)
			{
				QLOG_INFO() << item->physicsBody->GetAngle();
			}
*/
