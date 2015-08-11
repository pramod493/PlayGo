#include "cdState.h"
#include "PlayGoController.h"

namespace CDI
{
	cdState::cdState(PlayGoController *parentController, QState *parentState)
		: QState(parentState)
	{
		_controller = parentController;
		init();
	}

	cdState::cdState(PlayGoController *parentController, ChildMode childMode, QState *parentState)
		: QState(childMode, parentState)
	{
		_controller = parentController;
		init();
	}

	void cdState::init()
	{
		if (_controller)
		{
			_view		=_controller->_view;
			_scene		=static_cast<QGraphicsScene*>(_controller->_scene);
			_page		= _controller->_page;
		} else {
			_view = 0; _scene = 0; _page = 0;
		}
	}
}
