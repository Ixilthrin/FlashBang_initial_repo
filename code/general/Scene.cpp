#include "Scene.h"

#include "ImageReader.h"
#include <filesystem>
namespace fs = std::filesystem;

void Scene::add(int id, Rectangle *rectangle)
{
	_rectangles.insert(std::pair<int, Rectangle*>(id, rectangle));
	_ids.push_back(id);
}

int Scene::size()
{
	return _rectangles.size();
}

std::vector<int> Scene::getIds()
{
	return _ids;
}

Rectangle *Scene::get(int id)
{
	return _rectangles[id];
}

void Scene::bringToTop(int id)
{
	auto position = std::find(_ids.begin(), _ids.end(), id);
	if (position != _ids.end())
	{
		_ids.erase(position);
	}
	// Possibly counter-intuitive, but the last item
	// will draw on top of other items.
	_ids.push_back(id);
}

void Scene::addImageCard(int id, std::string imageFile, int x, int y)
{
	Rectangle *rectangle = new Rectangle{ 128, 128, x, y, imageFile,
		"c:/programming/FlashBang/shaders/translation_and_texture.vert.glsl",
		"c:/programming/FlashBang/shaders/texture.frag.glsl"
	};

	add(id, rectangle);
}

void Scene::addCards(std::vector<std::string> filenames)
{
	int x = 50;
	int y = 50;
	int id = 0;
	for (auto i = filenames.begin(); i < filenames.end(); ++i)
	{
		std::string path = "c:/programming/FlashBangProject/resources/" + *i;
		addImageCard(id, path, x, y);
		++id;
		x += 10;
		y += 10;
	}
}

void Scene::addCardsFromDirectory(std::string basepath)
{
	int x = 50;
	int y = 50;
	int id = 0;

	//std::string basepath = "c:/programming/FlashBangProject/resources/";
	for (const auto &entry : fs::directory_iterator(basepath))
	{
		std::string path{ entry.path().u8string() };
		if (path.find(".png", 0))
		{
			addImageCard(id, path, x, y);
			++id;
			x += 1;
			y += 1;
		}
	}
}