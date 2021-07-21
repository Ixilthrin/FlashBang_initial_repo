#include "CardImageData.h"

CardImageData::CardImageData(string path)
{
    _imageReader = new ImageReader(path);
    _backImageReader = 0;
}

void CardImageData::setBackPath(string path)
{
    _backImageReader = new ImageReader(path);
}

ImageReader *CardImageData::getImageReader()
{
    return _imageReader;
}

ImageReader *CardImageData::getBackImageReader()
{
    return _backImageReader;
}