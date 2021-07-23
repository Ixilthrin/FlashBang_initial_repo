#pragma once

#include "CardDeckInputListener.h"
#include "CardDeckEventTranslator.h"
#include "CardDeckDispatchingMouseHandlers.h" // Interesting: putting this after next line causes link errors
#include "Converter.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Card.h"
#include "CardDeck.h"

class BasicCardDeck
{
private:
    int _width;
    int _height;
    GLFWwindow* _window;
    CardDeck *_deck;
    CardDeckInputListener *_listener;
    CardDeckEventTranslator *_translator;
    GLint _translationLocation;
    GLint _rotationYLocation;
    GLint _objectWidthLocation; 
    GLuint _programHandle; 
    GLuint _positionBufferHandle; 
    GLuint _colorBufferHandle;
    GLuint _texCoordsBufferHandle;
    GLuint _indexCoordsBufferHandle;
    vector<unsigned int> _indexData;
    map<int, int> _indexOffsets;
    GLuint *_textureNames;
    map<int, GLuint> _textures;
    map<int, GLuint> _flippedTextures;
    Converter *_converter;
    float _currentFlipRotation;
    bool _flippedHalf;

    int initializeWindow();
    int initializeOpenGL();
    int loadDeck();
    int setupShaders();
    int setupBuffers();
    int setupIndexData();
    int setupBindings();
    int setupUniformLocations();
    int setupTextures();
    int setupEventListeners();
    int setupGlobalGraphicsState();

    void renderFrame();

    int cleanUp();
public:
    BasicCardDeck();
    virtual ~BasicCardDeck();
    int Start();
};

