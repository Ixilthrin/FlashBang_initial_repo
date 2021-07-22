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

#include "DispatchingMouseHandlers.h" // Interesting: putting this after next line causes link errors
#include <GLFW/glfw3.h>

#include "Scene.h"
#include "Card.h"
#include "CardGeometry.h"
#include "Converter.h"

#include "ShaderProgramFactory.h"


BasicCardDeck::BasicCardDeck()
{
}

BasicCardDeck::~BasicCardDeck()
{
}

int BasicCardDeck::Draw()
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;

    int width = 1600;
    int height = 900;
    window = glfwCreateWindow(width, height, "Simulated Card Deck Prototype", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -2;
    }

    glfwMakeContextCurrent(window); 


    if (!gladLoadGL())
        return -3;


    string targetFile;
    ifstream myfile("c:/programming/FlashBangProject/flashbang.props");
    if (myfile.is_open())
    {
        getline(myfile, targetFile);
        myfile.close();
    }

    string basedir = "c:/programming/FlashBangProject/" + targetFile + "/";

    Scene scene;

    scene.addCardsFromDirectory(basedir);
    scene.shuffle();

    Converter converter{ width, height };

    for (auto const& id : scene.getIds())
    {
        auto geometry = new CardGeometry(scene.get(id), &converter);
        scene.addGeometry(id, geometry);
    }

    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    if (vertShader == 0)
        return -4;

    GLuint programHandle = ShaderProgramFactory::BuildShaderProgram(
        scene.get(0)->getVertShaderPath(),
        scene.get(0)->getFragShaderPath()
    );

    glUseProgram(programHandle);

    //glDetachShader(programHandle, vertShader);
    //glDetachShader(programHandle, fragShader);

    //glDeleteShader(vertShader);
    //glDeleteShader(fragShader);

    InputListener listener;
    listener.setScene(&scene);

    int positionLength = 0;
    int colorLength = 0;
    int texCoordLength = 0; 
    
    for (auto const& id : scene.getIds())
    {
        positionLength += scene.getGeometry(id)->getPositions().size();
        colorLength += scene.getGeometry(id)->getColors().size();
        texCoordLength += scene.getGeometry(id)->getTexCoords().size();
    }

    GLuint vboHandles[4];
    glGenBuffers(4, vboHandles);

    GLuint positionBufferHandle = vboHandles[0];
    GLuint colorBufferHandle = vboHandles[1];
    GLuint texCoordsBufferHandle = vboHandles[2];
    GLuint indexCoordsBufferHandle = vboHandles[3];

    glNamedBufferData(positionBufferHandle, positionLength * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glNamedBufferData(colorBufferHandle, colorLength * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    glNamedBufferData(texCoordsBufferHandle, texCoordLength * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    int positionsOffset = 0;
    int colorsOffset = 0;
    int texCoordsOffset = 0;

    for (auto const& id : scene.getIds())
    {
        auto geometry = scene.getGeometry(id);

        auto positions = geometry->getPositions();
        auto positionsSize = positions.size() * sizeof(float);
        glNamedBufferSubData(positionBufferHandle, positionsOffset, positionsSize, positions.data());
        positionsOffset += positionsSize;

        auto colors = geometry->getColors();
        auto colorsSize = colors.size() * sizeof(float);
        glNamedBufferSubData(colorBufferHandle, colorsOffset, colorsSize, colors.data());
        colorsOffset += colorsSize;

        auto texCoords = geometry->getTexCoords();
        auto texCoordsSize = texCoords.size() * sizeof(float);
        glNamedBufferSubData(texCoordsBufferHandle, texCoordsOffset, texCoordsSize, texCoords.data());
        texCoordsOffset += texCoordsSize;
    }
    
    vector<unsigned int> indexData;

    unsigned int baseIndex = 0;

    map<int, int> indexOffsets;
    int currentIndexOffset = 0;
    for (auto const &id : scene.getIds())
    {
        auto geom = scene.getGeometry(id);
        auto currentIndices = geom->getIndexData();
        indexOffsets.insert(pair<int, int>(id, currentIndexOffset));
        currentIndexOffset += currentIndices.size();
        for (auto const &value : currentIndices)
        {
            auto index = value + baseIndex;
            indexData.push_back(index);
        }
        baseIndex += geom->getPositions().size() / 3;
    }

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

    glBindVertexBuffer(0, positionBufferHandle, 0, sizeof(GLfloat) * 3);
    glBindVertexBuffer(1, colorBufferHandle, 0, sizeof(GLfloat) * 3);
    glBindVertexBuffer(2, texCoordsBufferHandle, 0, sizeof(GLfloat) * 2);

    glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(0, 0);

    glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(1, 1);

    glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, 0);
    glVertexAttribBinding(2, 2);

    GLint translation = glGetUniformLocation(programHandle, "Translation");
    GLint rotationY = glGetUniformLocation(programHandle, "RotationY");
    GLint objectWidth = glGetUniformLocation(programHandle, "ObjectWidth");
    
    int numberOfTexturesNeeded = scene.numberOfCardSides();
    GLuint *textureNames = new GLuint[numberOfTexturesNeeded];
    glCreateTextures(GL_TEXTURE_2D, numberOfTexturesNeeded, textureNames);

    map<int, GLuint> textures;
    map<int, GLuint> flippedTextures;
    int textureNameIndex = 0;

    vector<int> toBeRemoved;
    for (auto const &id : scene.getIds()) 
    {
        auto imageReader = scene.getImageData(id)->getImageReader();
        GLubyte* image = imageReader->getImageData();
        int imageWidth = imageReader->getWidth();
        int imageHeight = imageReader->getHeight();

        if (image == 0 || imageWidth == 0 || imageHeight == 0)
        {
            toBeRemoved.push_back(id);
            continue;
        }

        textures.insert(pair<int, GLuint>(id, textureNames[textureNameIndex]));


        glTextureStorage2D(textureNames[textureNameIndex], 1, GL_RGBA8, imageWidth, imageHeight);
        glBindTexture(GL_TEXTURE_2D, textureNames[textureNameIndex]);

        if (image == NULL)
            throw(string("Failed to load image"));
        glTextureSubImage2D(textureNames[textureNameIndex], 0, 0, 0, imageWidth, imageHeight, GL_RGBA, GL_UNSIGNED_BYTE, image);

        ++textureNameIndex;
    }

    for (auto const &id : toBeRemoved)
    {
        scene.removeCard(id);
    }

    for (auto const &id : scene.getIds())
    {
        if (!scene.get(id)->hasFlipSide())
        {
            continue;
        }

        flippedTextures.insert(pair<int, GLuint>(id, textureNames[textureNameIndex]));

        auto reader = scene.getImageData(id)->getBackImageReader();

        GLubyte* image = reader->getImageData();
        int imageWidth = reader->getWidth();
        int imageHeight = reader->getHeight();

        glTextureStorage2D(textureNames[textureNameIndex], 1, GL_RGBA8, imageWidth, imageHeight);
        glBindTexture(GL_TEXTURE_2D, textureNames[textureNameIndex]);

        if (image == nullptr)
            throw(string("Failed to load image"));
        glTextureSubImage2D(textureNames[textureNameIndex], 0, 0, 0, imageWidth, imageHeight, GL_RGBA, GL_UNSIGNED_BYTE, image);

        ++textureNameIndex;
    }

    EventTranslator translator;

    DispatchingMouseHandlers::translator = &translator;
    translator.registerListener(&listener);
    glfwSetCursorPosCallback(window, DispatchingMouseHandlers::cursor_position_callback);
    glfwSetCursorEnterCallback(window, DispatchingMouseHandlers::cursor_enter_callback);
    glfwSetMouseButtonCallback(window, DispatchingMouseHandlers::mouse_button_callback);
    glfwSetScrollCallback(window, DispatchingMouseHandlers::scroll_callback);

    glfwSwapInterval(1);
    glBindVertexArray(vaoHandle);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glCullFace(GL_BACK);
    glClearColor(.6, .6, .6, 1);
    glPointSize(5);
    glLineWidth(3);
    glDisable(GL_DEPTH_TEST);

    float rot = 0;
    bool flippedHalf = false;
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        if (translation != -1)
        {
            float xTrans, yTrans;

            int objectCount = 0;
            vector<int> ids = scene.getIds();
            for (auto const& id : ids)
            {
                auto card = scene.get(id);
                if (card->hasFlipSide() && card->requestFlip())
                {
                    glUniform1f(rotationY, rot);
                    glUniform1f(objectWidth, converter.screenTranslationXToNDC(card->getWidth()));
                    rot += .15;
                    if (rot > 1.57 && !flippedHalf)
                    {
                        rot += 3.14f;
                        card->flipHalfComplete();
                        flippedHalf = true;
                    }
                    if (rot > 6.28)
                    {
                        card->flipComplete();
                        rot = 0;
                        flippedHalf = false;
                    }
                }
                else
                {
                    glUniform1f(rotationY, 0);
                }
                if (card->hasFlipSide() && card->isFlipped())
                {
                    glBindTextureUnit(0, flippedTextures[id]);
                }
                else
                {
                    glBindTextureUnit(0, textures[id]);
                }

                int selected = listener.getSelectedId();
                if (listener.isSelectAndMoveInProgress() && selected == id)
                {
                    xTrans = converter.screenTranslationXToNDC(scene.get(selected)->getTranslationX() + listener.getMovementX());
                    yTrans = converter.screenTranslationYToNDC(scene.get(selected)->getTranslationY() + listener.getMovementY());
                }
                else
                {
                    xTrans = converter.screenTranslationXToNDC(scene.get(id)->getTranslationX());
                    yTrans = converter.screenTranslationYToNDC(scene.get(id)->getTranslationY());
                }
                
                glUniform2f(translation, xTrans, yTrans);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, &(indexData.data())[indexOffsets[id]]);
            }

        }


        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glDeleteTextures(scene.size(), textureNames);
    delete[] textureNames;

    return 0;
}
