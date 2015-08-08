#ifndef SKETCHSTATE_H
#define SKETCHSTATE_H
#include "cdState.h"

namespace CDI
{
	class SketchState : public cdState
	{
	public:
		SketchState(PlayGoController* parentController, QState *parentState = 0);

	};
}
#endif // SKETCHSTATE_H
