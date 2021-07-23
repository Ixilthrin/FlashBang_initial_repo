#include "BasicCardDeck.h"

#include <iostream>
#include <glad/glad.h>
#include <stdio.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

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

#include "CardGeometry.h"
#include "ShaderProgramFactory.h"

BasicCardDeck::BasicCardDeck()
{
    _width = 1600;
    _height = 900;
    _textureNames = 0;
    _listener = new CardDeckInputListener();
    _deck = new CardDeck();
    _translator = new CardDeckEventTranslator();
}

BasicCardDeck::~BasicCardDeck()
{
}

int BasicCardDeck::Start()
{
    int result = initializeWindow();
    if (result != 0)
        return result;

    result = initializeOpenGL();
    if (result != 0)
        return result;

    loadDeck();

    result = setupShaders();
    if (result != 0)
        return result;

    setupBuffers();
    setupIndexData();
    setupBindings();
    setupUniformLocations();
    setupTextures();
    setupEventListeners();
    setupGlobalGraphicsState();

    _currentFlipRotation = 0;
    _flippedHalf = false;

    while (!glfwWindowShouldClose(_window))
    {
        renderFrame();
        glfwSwapBuffers(_window);
        glfwPollEvents();
    }

    cleanUp();

    return 0;
}

int BasicCardDeck::initializeWindow()
{
    if (!glfwInit())
        return -1;

    _window = glfwCreateWindow(_width, _height, "Simulated Card Deck Prototype", NULL, NULL);
    if (!_window)
    {
        glfwTerminate();
        return -2;
    }

    glfwMakeContextCurrent(_window);

    return 0;
}

int BasicCardDeck::initializeOpenGL()
{
    if (!gladLoadGL())
        return -3;
    return 0;
}

int BasicCardDeck::loadDeck()
{
    string targetFile;
    ifstream myfile("c:/programming/FlashBangProject/flashbang.props");
    if (myfile.is_open())
    {
        getline(myfile, targetFile);
        myfile.close();
    }
    string baseDir = "c:/programming/FlashBangProject/decks/" + targetFile + "/";

    _converter = new Converter(_width, _height);
    _deck->setConverter(_converter);
    _deck->setUpFromBaseDir(baseDir);

	glfwSetWindowTitle(_window, targetFile.c_str());
    return 0;
}

int BasicCardDeck::setupShaders()
{
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    if (vertShader == 0)
        return -4;

    _programHandle = ShaderProgramFactory::BuildShaderProgram(
        Card::getVertexShaderPath(),
        Card::getFragmentShaderPath()
    );

    glUseProgram(_programHandle);

    //glDetachShader(_programHandle, vertShader);
    //glDetachShader(_programHandle, fragShader);

    //glDeleteShader(vertShader);
    //glDeleteShader(fragShader);
    return 0;
}

int BasicCardDeck::setupBuffers()
{
    int positionLength = 0;
    int colorLength = 0;
    int texCoordLength = 0;

    for (auto const& id : _deck->getIds())
    {
        positionLength += _deck->getGeometry(id)->getPositions().size();
        colorLength += _deck->getGeometry(id)->getColors().size();
        texCoordLength += _deck->getGeometry(id)->getTexCoords().size();
    }

    GLuint vboHandles[4];
    glGenBuffers(4, vboHandles);

    _positionBufferHandle = vboHandles[0];
    _colorBufferHandle = vboHandles[1];
    _texCoordsBufferHandle = vboHandles[2];
    _indexCoordsBufferHandle = vboHandles[3];

    glNamedBufferData(_positionBufferHandle, positionLength * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glNamedBufferData(_colorBufferHandle, colorLength * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glNamedBufferData(_texCoordsBufferHandle, texCoordLength * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    int positionsOffset = 0;
    int colorsOffset = 0;
    int texCoordsOffset = 0;

    for (auto const& id : _deck->getIds())
    {
        auto geometry = _deck->getGeometry(id);

        auto positions = geometry->getPositions();
        auto positionsSize = positions.size() * sizeof(float);
        glNamedBufferSubData(_positionBufferHandle, positionsOffset, positionsSize, positions.data());
        positionsOffset += positionsSize;

        auto colors = geometry->getColors();
        auto colorsSize = colors.size() * sizeof(float);
        glNamedBufferSubData(_colorBufferHandle, colorsOffset, colorsSize, colors.data());
        colorsOffset += colorsSize;

        auto texCoords = geometry->getTexCoords();
        auto texCoordsSize = texCoords.size() * sizeof(float);
        glNamedBufferSubData(_texCoordsBufferHandle, texCoordsOffset, texCoordsSize, texCoords.data());
        texCoordsOffset += texCoordsSize;
    }

    return 0;
}

int BasicCardDeck::setupIndexData()
{
    _deck->getIndexData(_indexData, _indexOffsets);
    return 0;
}

int BasicCardDeck::setupBindings()
{
    // I don't know why the following 2 lines did not work, but instead of
    // binding an index buffer I just use the array of index values.

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexCoordsBufferHandle);
    //glBufferData(indexCoordsBufferHandle, indexData.size() * sizeof(unsigned int), indexData.data(), GL_STATIC_DRAW);

    GLuint vaoHandle;
    glGenVertexArrays(1, &vaoHandle);
    glBindVertexArray(vaoHandle);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexBuffer(0, _positionBufferHandle, 0, sizeof(GLfloat) * 3);
    glBindVertexBuffer(1, _colorBufferHandle, 0, sizeof(GLfloat) * 3);
    glBindVertexBuffer(2, _texCoordsBufferHandle, 0, sizeof(GLfloat) * 2);

    glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(0, 0);

    glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(1, 1);

    glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(2, 2);

    glBindVertexArray(vaoHandle);

    return 0;
}

int BasicCardDeck::setupUniformLocations()
{
    _translationLocation = glGetUniformLocation(_programHandle, "Translation");
    _rotationYLocation = glGetUniformLocation(_programHandle, "RotationY");
    _objectWidthLocation = glGetUniformLocation(_programHandle, "ObjectWidth");

    return 0;
}

int BasicCardDeck::setupTextures()
{
    int numberOfTexturesNeeded = _deck->numberOfCardSides();
    _textureNames = new GLuint[numberOfTexturesNeeded];
    glCreateTextures(GL_TEXTURE_2D, numberOfTexturesNeeded, _textureNames);

    int textureNameIndex = 0;

    vector<int> toBeRemoved;
    for (auto const &id : _deck->getIds())
    {
        auto imageReader = _deck->getImageData(id)->getImageReader();
        GLubyte* image = imageReader->getImageData();
        int imageWidth = imageReader->getWidth();
        int imageHeight = imageReader->getHeight();

        if (image == 0 || imageWidth == 0 || imageHeight == 0)
        {
            toBeRemoved.push_back(id);
            continue;
        }

        _textures.insert(pair<int, GLuint>(id, _textureNames[textureNameIndex]));


        glTextureStorage2D(_textureNames[textureNameIndex], 1, GL_RGBA8, imageWidth, imageHeight);
        glBindTexture(GL_TEXTURE_2D, _textureNames[textureNameIndex]);

        if (image == NULL)
            throw(string("Failed to load image"));
        glTextureSubImage2D(_textureNames[textureNameIndex], 0, 0, 0, imageWidth, imageHeight, GL_RGBA, GL_UNSIGNED_BYTE, image);

        ++textureNameIndex;
    }

    for (auto const &id : toBeRemoved)
    {
        _deck->removeCard(id);
    }

    for (auto const &id : _deck->getIds())
    {
        if (!_deck->get(id)->hasFlipSide())
        {
            continue;
        }

        _flippedTextures.insert(pair<int, GLuint>(id, _textureNames[textureNameIndex]));

        auto reader = _deck->getImageData(id)->getBackImageReader();

        GLubyte* image = reader->getImageData();
        int imageWidth = reader->getWidth();
        int imageHeight = reader->getHeight();

        glTextureStorage2D(_textureNames[textureNameIndex], 1, GL_RGBA8, imageWidth, imageHeight);
        glBindTexture(GL_TEXTURE_2D, _textureNames[textureNameIndex]);

        if (image == nullptr)
            throw(string("Failed to load image"));
        glTextureSubImage2D(_textureNames[textureNameIndex], 0, 0, 0, imageWidth, imageHeight, GL_RGBA, GL_UNSIGNED_BYTE, image);

        ++textureNameIndex;
    }
    return 0;
}

int BasicCardDeck::cleanUp()
{
    if (_textureNames)
    {
        glDeleteTextures(_deck->size(), _textureNames);
        delete[] _textureNames;
    }

    return 0;
}

int BasicCardDeck::setupEventListeners()
{
    _listener->setDeck(_deck);
    CardDeckDispatchingMouseHandlers::translator = _translator;
    _translator->registerListener(_listener);
    glfwSetCursorPosCallback(_window, CardDeckDispatchingMouseHandlers::cursor_position_callback);
    glfwSetCursorEnterCallback(_window, CardDeckDispatchingMouseHandlers::cursor_enter_callback);
    glfwSetMouseButtonCallback(_window, CardDeckDispatchingMouseHandlers::mouse_button_callback);
    glfwSetScrollCallback(_window, CardDeckDispatchingMouseHandlers::scroll_callback);
    return 0;
}

int BasicCardDeck::setupGlobalGraphicsState()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);
    glClearColor(.6, .6, .6, 1);
    glPointSize(5);
    glLineWidth(3);
    glDisable(GL_DEPTH_TEST);
    glfwSwapInterval(1);
    return 0;
}

void BasicCardDeck::renderFrame()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (_translationLocation != -1)
    {
        float xTrans, yTrans;

        int objectCount = 0;
        vector<int> ids = _deck->getIds();
        for (auto const& id : ids)
        {
            auto card = _deck->get(id);
            if (card->hasFlipSide() && card->requestFlip())
            {
                glUniform1f(_rotationYLocation, _currentFlipRotation);
                glUniform1f(_objectWidthLocation, _converter->screenTranslationXToNDC(card->getWidth()));
                _currentFlipRotation += .15;
                if (_currentFlipRotation > 1.57 && !_flippedHalf)
                {
                    _currentFlipRotation += 3.14f;
                    card->flipHalfComplete();
                    _flippedHalf = true;
                }
                if (_currentFlipRotation > 6.28)
                {
                    card->flipComplete();
                    _currentFlipRotation = 0;
                    _flippedHalf = false;
                }
            }
            else
            {
                glUniform1f(_rotationYLocation, 0);
            }
            if (card->hasFlipSide() && card->isFlipped())
            {
                glBindTextureUnit(0, _flippedTextures[id]);
            }
            else
            {
                glBindTextureUnit(0, _textures[id]);
            }

            int selected = _listener->getSelectedId();
            if (_listener->isSelectAndMoveInProgress() && selected == id)
            {
                xTrans = _converter->screenTranslationXToNDC(_deck->get(selected)->getTranslationX() + _listener->getMovementX());
                yTrans = _converter->screenTranslationYToNDC(_deck->get(selected)->getTranslationY() + _listener->getMovementY());
            }
            else
            {
                xTrans = _converter->screenTranslationXToNDC(_deck->get(id)->getTranslationX());
                yTrans = _converter->screenTranslationYToNDC(_deck->get(id)->getTranslationY());
            }

            glUniform2f(_translationLocation, xTrans, yTrans);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, &(_indexData.data())[_indexOffsets[id]]);
        }
    }
}