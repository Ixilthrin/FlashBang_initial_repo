#include "InputListener.h"

#include <iostream>
#include <vector>
using std::vector;

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
	vector<int> ids = _scene->getIds();
	auto it = ids.rbegin();
	while (it != ids.rend())
	{
		Card *card = _scene->get(*it);
		if (card && card->contains(x, y))
		{
			if (!_selectAndMoveInProgress)
			{
				_selectionStartX = x;
				_selectionStartY = y;
				_mouseX = x;
				_mouseY = y;
			}
			_selectedId = *it;
			_scene->bringToTop(*it);
			_selectAndMoveInProgress = true;
			break;
		}
		it++;
	}
}

void InputListener::moveSelection(int x, int y)
{
	Card *card = _scene->get(_selectedId);
	if (_selectAndMoveInProgress)
	{
		_mouseX = x;
		_mouseY = y;
	}
	else if (card && card->contains(x, y))
	{
		//std::cout << "Mouse Over" << std::endl;
	}
}

void InputListener::endSelect(int x, int y)
{
	Card *card = _scene->get(_selectedId);
	if (_selectAndMoveInProgress && card)
	{
		card->setTranslationX(card->getTranslationX() + _mouseX - _selectionStartX);
		card->setTranslationY(card->getTranslationY() + _mouseY - _selectionStartY);
		_mouseX = 0;
		_mouseY = 0;
		_selectionStartX = 0;
		_selectionStartY = 0;
	}
	_selectAndMoveInProgress = false;
	_selectedId = -1;
}

void InputListener::flip(int x, int y)
{
	auto ids = _scene->getIds();
	auto it = ids.rbegin();
	while (it != ids.rend())
	{
		Card *card = _scene->get(*it);
		if (card && card->contains(x, y))
		{
			card->flip();
			_scene->bringToTop(*it);
			break;
		}
		it++;
	}
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