#include "cdcontactlistener.h"
#include "cdi2qtwrapper.h"
#include "QsLog.h"

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
		QLOG_INFO() << "Begin contact";
		Q_UNUSED(contact)
	}

	void cdContactListener::EndContact(b2Contact *contact)
	{
		QLOG_INFO() << "End contact";
		Q_UNUSED(contact)
	}

	void cdContactListener::PreSolve(b2Contact *contact, const b2Manifold *oldManifold)
	{
		QLOG_INFO() << "Pre solve";
		Q_UNUSED(contact)
		Q_UNUSED(oldManifold)
	}

	void cdContactListener::PostSolve(b2Contact *contact, const b2ContactImpulse *impulse)
	{
		QLOG_INFO() << "Post solve";
		Q_UNUSED(contact)
		Q_UNUSED(impulse)
	}
}

