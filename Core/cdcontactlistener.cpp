#include "cdcontactlistener.h"
#include "cdi2qtwrapper.h"

namespace CDI
{
	cdContactListener::cdContactListener()
	{

	}

	cdContactListener::~cdContactListener()
	{

	}

	void cdContactListener::BeginContact(b2Contact *contact)
	{
		qDebug() << "Begin contact";
		Q_UNUSED(contact)
	}

	void cdContactListener::EndContact(b2Contact *contact)
	{
		qDebug() << "End contact";
		Q_UNUSED(contact)
	}

	void cdContactListener::PreSolve(b2Contact *contact, const b2Manifold *oldManifold)
	{
		qDebug() << "Pre solve";
		Q_UNUSED(contact)
		Q_UNUSED(oldManifold)
	}

	void cdContactListener::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse)
	{
		qDebug() << "Post solve";
		Q_UNUSED(contact)
		Q_UNUSED(impulse)
	}
}

