#pragma once

#include "Scene.h"
#include "Rectangle.h"

class InputListener
{
private:
	int _mouseX;
	int _mouseY;
	bool _selectAndMoveInProgress;
	Scene *_scene;
	int _selectionStartX;
	int _selectionStartY;
	int _selectedId;

public:
	InputListener();
	inline void setScene(Scene *scene)
	{
		_scene = scene;
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
	int getSelectedId();
};
