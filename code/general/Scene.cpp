#include "Scene.h"
#include "CardGeometry.h"

#include "ImageReader.h"
#include <filesystem>
namespace fs = std::filesystem;

#include <iostream>

void Scene::add(int id, Card *card)
{
    _cards.insert(pair<int, Card*>(id, card));
    _ids.push_back(id);
}

void Scene::removeCard(int id)
{
	_cards.erase(id);
	_ids.erase(std::remove(_ids.begin(), _ids.end(), id));
}

void Scene::addGeometry(int id, CardGeometry *geometry)
{
    _geometry.insert(pair<int, CardGeometry*>(id, geometry));
}

void Scene::addImageData(int id, CardImageData *data)
{
    _imageData.insert(pair<int, CardImageData*>(id, data));
}

int Scene::size()
{
    return _cards.size();
}

int Scene::numberOfCardSides()
{
    int numberOfSides = 0;
    for (auto const& cardPair : _cards)
    {
        ++numberOfSides;
        auto card = cardPair.second;
        if (card->hasFlipSide())
            ++numberOfSides;
    }
    return numberOfSides;
}

vector<int> Scene::getIds()
{
    return _ids;
}

Card *Scene::get(int id)
{
    return _cards[id];
}

CardGeometry* Scene::getGeometry(int id)
{
    return _geometry[id];
}

CardImageData* Scene::getImageData(int id)
{
    return _imageData[id];
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

void Scene::addImageCard(int id, int x, int y,
    string imageFile, string backImageFile)
{
    CardImageData *imageData = new CardImageData(imageFile);
    imageData->setBackPath(backImageFile);

    int width = imageData->getImageReader()->getWidth();
    int height = imageData->getImageReader()->getHeight();

	int maxWidth = 300;
    if (width > maxWidth)
    {
        height = height / (width / maxWidth);
        width = maxWidth;
    }

    Card *card = new Card{ width, height, x, y, imageFile,
        "c:/programming/FlashBang/shaders/translation_and_texture.vert.glsl",
        "c:/programming/FlashBang/shaders/texture.frag.glsl"
    };

    if (backImageFile.length() > 0)
    {
        card->setFlippedImagePath(backImageFile);
        cout << "Back image = " << backImageFile << endl;
    }

    add(id, card);

    addImageData(id, imageData);
}

void Scene::addCards(vector<string> filenames)
{
    int x = 50;
    int y = 50;
    int id = 0;

    for (auto i = filenames.begin(); i < filenames.end(); ++i)
    {
        string path = "c:/programming/FlashBangProject/resources/" + *i;
        addImageCard(id, x, y, path, "");
        ++id;
        x += 10;
        y += 10;
    }
}

void Scene::addCardsFromDirectory(string basepath)
{
    int x = 50;
    int y = 50;
    int id = 0;

    vector<string> imageFrontNames;
    vector<string> imageBackNames;
	string extension = "";
    for (const auto &entry : fs::directory_iterator(basepath))
    {
        string path{ entry.path().u8string() };
        int pos = path.find(".png", 0);
		if (pos > 0)
		{
			extension = ".png";
		}

		// The stb image loader doesn't always work for jpg files
		// so disabling jpg support for now
		//else
		//{
		//	pos = path.find(".jpg", 0);
		//	if (pos > 0)
		//		extension = ".jpg";
		//}
			
        if (pos > 0)
        {
            auto substring = path.substr(0, path.length() - extension.length());
            int posBack = substring.find("-back", 0);
            if (posBack > 0)
                imageBackNames.push_back(substring);
            else
                imageFrontNames.push_back(substring);
        }
    }

    map<string, string> frontBackMap;

    for (const auto &front : imageFrontNames)
    {
        for (const auto &back : imageBackNames)
        {
            if (front == back.substr(0, front.length()))
                frontBackMap.insert(pair<string, string>(front, back));
        }
    }

    for (const auto &name : imageFrontNames)
    {
        bool hasBack = frontBackMap.count(name) > 0;
        string backPath;
        if (hasBack)
        {
            backPath = frontBackMap[name] + extension;
        }
        addImageCard(id, x, y, name + extension, backPath);
        ++id;
        x += 1;
        y += 1;
    }
}