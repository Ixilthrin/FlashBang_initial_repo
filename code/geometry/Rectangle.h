#pragma once

class Rectangle
{
private:
	int _width;
	int _height;
	int _translationX;
	int _translationY;

public:
	Rectangle(int width, int height, int translationX, int translationY);
	void setTranslationX(int translation);
	void setTranslationY(int translation);
	int getWidth();
	int getHeight();
	int getTranslationX();
	int getTranslationY();
	bool contains(int x, int y);
};
