#pragma once

#include "InputListener.h"


class EventTranslator
{
private:
	int _mouseX;
	int _mouseY;
	InputListener *_listener;

public:
	enum EventType
	{
		LeftMouseDown,
		LeftMouseUp,
		RightMouseDown,
		RightMouseUp
	};

	void registerListener(InputListener *listener);
	void mouseMoved(int x, int y);
	void translateMouseEvent(EventType type);

};