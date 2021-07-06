#include "InputListener.h"

#include <iostream>
#include <vector>

InputListener::InputListener()
{
	_selectAndMoveInProgress = false;
	_mouseX = 0;
	_mouseY = 0;
	_selectionStartX = 0;
	_selectionStartY = 0;
	_selectedId = -1;
}

void InputListener::select(int x, int y)
{
	std::vector<int> ids = _scene->getIds();
	for (auto i = ids.begin(); i < ids.end(); i++)
	{
		Rectangle *rect = _scene->get(*i);
		if (rect && rect->contains(x, y))
		{
			if (!_selectAndMoveInProgress)
			{
				_selectionStartX = x;
				_selectionStartY = y;
				_mouseX = x;
				_mouseY = y;
			}
			_selectedId = *i;
			_scene->bringToTop(*i);
			_selectAndMoveInProgress = true;
		}
	}
}

void InputListener::moveSelection(int x, int y)
{
	Rectangle *rect = _scene->get(_selectedId);
	if (_selectAndMoveInProgress)
	{
		_mouseX = x;
		_mouseY = y;
	}
	else if (rect && rect->contains(x, y))
	{
		std::cout << "Mouse Over" << std::endl;
	}
}

void InputListener::endSelect(int x, int y)
{
	Rectangle *rect = _scene->get(_selectedId);
	if (_selectAndMoveInProgress && rect)
	{
		rect->setTranslationX(rect->getTranslationX() + _mouseX - _selectionStartX);
		rect->setTranslationY(rect->getTranslationY() + _mouseY - _selectionStartY);
		_mouseX = 0;
		_mouseY = 0;
		_selectionStartX = 0;
		_selectionStartY = 0;
	}
	_selectAndMoveInProgress = false;
	_selectedId = -1;
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

int InputListener::getSelectedId()
{
	return _selectedId;
}