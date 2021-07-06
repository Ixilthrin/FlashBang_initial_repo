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
	std::string _imagePath;
	std::string _vertShaderPath;
	std::string _fragShaderPath;

public:
	Rectangle(int width,
		int height,
		int translationX,
		int translationY,
		std::string imagePath = "",
		std::string vertShaderPath = "",
		std::string fragShaderPath = "");

	void setTranslationX(int translation);
	void setTranslationY(int translation);
	int getWidth();
	int getHeight();
	int getTranslationX();
	int getTranslationY();
	std::string getImagePath();
	bool contains(int x, int y);
	std::vector<int> getIndexData();
	std::string getVertShaderPath();
	std::string getFragShaderPath();
};
