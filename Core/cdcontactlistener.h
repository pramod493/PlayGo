#ifndef CDCONTACTLISTENER_H
#define CDCONTACTLISTENER_H
#include <QObject>
#include "box2dworld.h"
#include "commonfunctions.h"

namespace CDI
{
	class PhysicsManager;
	/**
	 * @brief The cdContactListener class listens to all the collisions between
	 * bodies and allows other objects to interfere with this behavior
	 */
	class cdContactListener :public QObject, public b2ContactListener
	{
		Q_OBJECT
	public:
		/**
		 * @brief cdContactListener creates a derived class from
		 * b2ContactListener
		 */
		cdContactListener(QObject *parent = nullptr);

		virtual ~cdContactListener();

		void BeginContact(b2Contact* contact);

		void EndContact(b2Contact* contact);

		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);

		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
	};
}
#endif // CDCONTACTLISTENER_H
