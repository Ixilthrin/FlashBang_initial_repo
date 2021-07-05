#include "InputListener.h"

#include <iostream>

InputListener::InputListener()
{
	_selectAndMoveInProgress = false;
	_mouseX = 0;
	_mouseY = 0;
	_selectionStartX = 0;
	_selectionStartY = 0;
}

void InputListener::select(int x, int y)
{
	if (_rectangle && _rectangle->contains(x, y))
	{
		if (!_selectAndMoveInProgress)
		{
			_selectionStartX = x;
			_selectionStartY = y;
			_mouseX = x;
			_mouseY = y;
		}
		_selectAndMoveInProgress = true;
	}
}

void InputListener::moveSelection(int x, int y)
{
	if (_selectAndMoveInProgress)
	{
		_mouseX = x;
		_mouseY = y;
	}
	else if (_rectangle && _rectangle->contains(x, y))
	{
		std::cout << "Mouse Over" << std::endl;
	}
}

void InputListener::endSelect(int x, int y)
{
	if (_selectAndMoveInProgress && _rectangle)
	{
		_rectangle->setTranslationX(_rectangle->getTranslationX() + _mouseX - _selectionStartX);
		_rectangle->setTranslationY(_rectangle->getTranslationY() + _mouseY - _selectionStartY);
		_mouseX = 0;
		_mouseY = 0;
		_selectionStartX = 0;
		_selectionStartY = 0;
	}
	_selectAndMoveInProgress = false;
}

bool InputListener::isSelectAndMoveInProgress()
{
	return _selectAndMoveInProgress;
}

int InputListener::getMouseX()
{
	return _mouseX;
}

int InputListener::getMouseY()
{
	return _mouseY;
}
