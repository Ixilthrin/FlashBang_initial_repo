#include "EventTranslator.h"

void EventTranslator::registerListener(InputListener *listener)
{
	_listener = listener;
}

void EventTranslator::mouseMoved(int x, int y)
{
	_listener->moveSelection(x, y);
	_mouseX = x;
	_mouseY = y;
}

void EventTranslator::translateMouseEvent(EventType type)
{
	if (_listener && type == LeftMouseDown)
	{
		_listener->select(_mouseX, _mouseY);
	}
	else if (_listener && type == LeftMouseUp)
	{
		_listener->endSelect(_mouseX, _mouseY);
	}
	else if (_listener && type == RightMouseUp)
	{
		_listener->flip(_mouseX, _mouseY);
	}
}