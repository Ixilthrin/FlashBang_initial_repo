#pragma once

#include "Rectangle.h"

class InputListener
{
private:
	int _mouseX;
	int _mouseY;
	bool _selectAndMoveInProgress;
	Rectangle *_rectangle;
	int _selectionStartX;
	int _selectionStartY;

public:
	InputListener();
	inline void setRectangle(Rectangle *r)
	{
		_rectangle = r;
	}

	inline int getMovementX()
	{
		return _mouseX - _selectionStartX;
	}

	inline int getMovementY()
	{
		return _mouseY - _selectionStartY;
	}

	void select(int x, int y);
	void moveSelection(int x, int y);
	void endSelect(int x, int y);

	bool isSelectAndMoveInProgress();
	int getMouseX();
	int getMouseY();
};
