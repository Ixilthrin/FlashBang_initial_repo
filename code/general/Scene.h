#pragma once

#include <map>
#include <vector>
#include <string>
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
	void addImageCard(int id, std::string imagePath, int x, int y);
	void addCards(std::vector<std::string> filenames);
	void addCardsFromDefaultDirectory();
};
