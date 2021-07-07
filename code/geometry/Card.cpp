#include "Card.h"

#include <iostream>
using std::cout;
using std::endl;

Card::Card(
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
	_flippedImagePath = "";
	_vertShaderPath = vertShaderPath;
	_fragShaderPath = fragShaderPath;
	_isFlipped = false;
}

void Card::setTranslationX(int translation)
{
	_translationX = translation;
}

int Card::getWidth()
{
	return _width;
}

int Card::getHeight()
{
	return _height;
}

void Card::setTranslationY(int translation)
{
	_translationY = translation;
}

int Card::getTranslationX()
{
	return _translationX;
}

int Card::getTranslationY()
{
	return _translationY;
}

std::string Card::getImagePath()
{
	return _imagePath;
}

std::string Card::getFlippedImagePath()
{
	return _flippedImagePath;
}

void Card::setFlippedImagePath(std::string path)
{
	_flippedImagePath = path;
}

bool Card::hasFlipSide()
{
	return _flippedImagePath.length() > 0;
}

void Card::flip()
{
	_isFlipped = !_isFlipped;
	cout << "setting _isflipped" << _isFlipped << endl;
}

bool Card::isFlipped()
{
	return _isFlipped;
}

bool Card::contains(int x, int y)
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

std::vector<int> Card::getIndexData()
{
	std::vector<int> indexData
	{
		0, 1, 2,
		2, 3, 0
	};
	return indexData;
}

std::string Card::getVertShaderPath()
{
	return _vertShaderPath;
}

std::string Card::getFragShaderPath()
{
	return _fragShaderPath;
}