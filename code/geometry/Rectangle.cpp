#include "Rectangle.h"


Rectangle::Rectangle(int width, int height, int translationX, int translationY)
{
	_width = width;
	_height = height;
	_translationX = translationX;
	_translationY = translationY;
}

void Rectangle::setTranslationX(int translation)
{
	_translationX = translation;
}

int Rectangle::getWidth()
{
	return _width;
}

int Rectangle::getHeight()
{
	return _height;
}

void Rectangle::setTranslationY(int translation)
{
	_translationY = translation;
}

int Rectangle::getTranslationX()
{
	return _translationX;
}

int Rectangle::getTranslationY()
{
	return _translationY;
}

bool Rectangle::contains(int x, int y)
{
	if (x > _translationX
		&& y > _translationY
		&& x < _translationX + _width
		&& y < _translationY + _height)
	{
		return true;
	}

	return false;
}
