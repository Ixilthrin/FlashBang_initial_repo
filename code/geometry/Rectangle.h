#pragma once

#include "ImageReader.h"
#include <string>
#include <vector>

class Rectangle
{
private:
	int _width;
	int _height;
	int _translationX;
	int _translationY;
	ImageReader *_image;
	std::string _vertShaderPath;
	std::string _fragShaderPath;

public:
	Rectangle(int width,
		int height,
		int translationX,
		int translationY,
		ImageReader *image = 0,
		std::string vertShaderPath = "",
		std::string fragShaderPath = "");

	void setTranslationX(int translation);
	void setTranslationY(int translation);
	int getWidth();
	int getHeight();
	int getTranslationX();
	int getTranslationY();
	ImageReader *getImage();
	bool contains(int x, int y);
	std::vector<int> getIndexData();
	std::string getVertShaderPath();
	std::string getFragShaderPath();
};
