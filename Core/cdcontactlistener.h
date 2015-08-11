#ifndef CDCONTACTLISTENER_H
#define CDCONTACTLISTENER_H
#include "box2dworld.h"
#include "commonfunctions.h"

namespace CDI
{
	class PhysicsManager;

	class cdContactListener : public b2ContactListener
	{
	public:
		cdContactListener();

		virtual ~cdContactListener();

		void BeginContact(b2Contact* contact);

		void EndContact(b2Contact* contact);

		void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);

		void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

		friend class PhysicsManager;
	};
}
#endif // CDCONTACTLISTENER_H
