#include "Rectangle.h"

Rectangle::Rectangle(
	int width, 
	int height, 
	int translationX, 
	int translationY, 
	std::string imagePath,
	std::string vertShaderPath,
	std::string fragShaderPath)
{
	_width = width;
	_height = height;
	_translationX = translationX;
	_translationY = translationY;
	_imagePath = imagePath;
	_vertShaderPath = vertShaderPath;
	_fragShaderPath = fragShaderPath;
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

std::string Rectangle::getImagePath()
{
	return _imagePath;
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

std::vector<int> Rectangle::getIndexData()
{
	std::vector<int> indexData
	{
		0, 1, 2,
		2, 3, 0
	};
	return indexData;
}

std::string Rectangle::getVertShaderPath()
{
	return _vertShaderPath;
}

std::string Rectangle::getFragShaderPath()
{
	return _fragShaderPath;
}