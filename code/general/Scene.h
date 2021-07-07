#pragma once

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include "Card.h"

using std::endl;
using std::cout;
using std::cin;
using std::cerr;
using std::string;
using std::ifstream;
using std::stringstream;
using std::map;
using std::pair;
using std::vector;

class Scene
{
private:
	vector<int> _ids;
	map<int, Card*> _cards;
public:
	void add(int id, Card *card);
	int size();
	int numberOfCardSides();
	vector<int> getIds();
	Card *get(int id);
	void bringToTop(int id);
	void addImageCard(int id, int x, int y, 
		string imagePath, string backImagePath);
	void addCards(vector<string> filenames);
	void addCardsFromDirectory(string basepath);
};
