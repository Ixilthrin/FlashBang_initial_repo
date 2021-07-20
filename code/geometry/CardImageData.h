#pragma once

#include <string>
#include "ImageReader.h"

using std::string;

class CardImageData
{
private:
	ImageReader *_imageReader;
	ImageReader *_backImageReader;
public:
	CardImageData(string path);
	void setBackPath(string path);
	ImageReader *getImageReader();
	ImageReader *getBackImageReader();
};