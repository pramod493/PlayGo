#include "sketchstate.h"

namespace CDI
{
	SketchState::SketchState
	(PlayGoController* parentController, QState *parentState)
		: cdState(parentController, parentState)
	{
		Q_UNUSED(parentController)
	}

}
