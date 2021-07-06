#pragma once

#include <map>
#include <vector>
#include "Rectangle.h"

class Scene
{
private:
	std::vector<int> _ids;
	std::map<int, Rectangle*> _rectangles;
public:
	void add(int id, Rectangle *rectangle);
	int size();
	std::vector<int> getIds();
	Rectangle *get(int id);
	void bringToTop(int id);
};
