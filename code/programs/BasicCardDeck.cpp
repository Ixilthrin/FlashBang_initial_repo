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

	Card *card = scene.get(0);
	auto positions = scene.getGeometry(0)->getPositions();
	auto colors = scene.getGeometry(0)->getColors();
	auto texCoords = scene.getGeometry(0)->getTexCoords();

	int positionLength = positions.size();
	int colorDataLength = colors.size();
	int texCoordLength = texCoords.size();

	GLuint vboHandles[3];
	glGenBuffers(3, vboHandles);

	GLuint positionBufferHandle = vboHandles[0];
	GLuint colorBufferHandle = vboHandles[1];
	GLuint texCoordsBufferHandle = vboHandles[2];

	glNamedBufferData(positionBufferHandle, positionLength * sizeof(float), positions.data(), GL_STATIC_DRAW);

	glNamedBufferData(colorBufferHandle, colorDataLength * sizeof(float), colors.data(), GL_STATIC_DRAW);

	glNamedBufferData(texCoordsBufferHandle, texCoordLength * sizeof(float), texCoords.data(), GL_STATIC_DRAW);

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

	GLint loc = glGetUniformLocation(programHandle, "Translation");

	cout << "scene size: " << scene.size() << "     sides: " << scene.numberOfCardSides() << endl;
	
	int numberOfTexturesNeeded = scene.numberOfCardSides();
	GLuint *textureNames = new GLuint[numberOfTexturesNeeded];
	glCreateTextures(GL_TEXTURE_2D, numberOfTexturesNeeded, textureNames);

	map<int, GLuint> textures;
	map<int, GLuint> flippedTextures;
	int textureNameIndex = 0;

	auto ids = scene.getIds();

	for (auto const &id : ids) 
	{
		cout << "texture name: " << textureNames[textureNameIndex] << endl;
		textures.insert(pair<int, GLuint>(id, textureNames[textureNameIndex]));

		auto imageReader = scene.getImageData(id)->getImageReader();
		GLubyte* image = imageReader->getImageData();
		int imageWidth = imageReader->getWidth();
		int imageHeight = imageReader->getHeight();
		cout << "width: " << imageWidth << "   height: " << imageHeight << "   comp:  " << imageReader->getComponentCount() << endl;

		glTextureStorage2D(textureNames[textureNameIndex], 1, GL_RGBA8, imageWidth, imageHeight);
		glBindTexture(GL_TEXTURE_2D, textureNames[textureNameIndex]);

		if (image == nullptr)
			throw(string("Failed to load image"));
		glTextureSubImage2D(textureNames[textureNameIndex], 0, 0, 0, imageWidth, imageHeight, GL_RGBA, GL_UNSIGNED_BYTE, image);

		++textureNameIndex;
	}

	for (auto const &id : ids)
	{
		if (!scene.get(id)->hasFlipSide())
		{
			continue;
		}

		cout << "texture name: " << textureNames[textureNameIndex] << endl;
		flippedTextures.insert(pair<int, GLuint>(id, textureNames[textureNameIndex]));
		cout << "mapping flip side to " << textureNames[textureNameIndex] << " at index " << textureNameIndex << endl;

		auto reader = scene.getImageData(id)->getBackImageReader();
		cout << "flipped image path: " << scene.get(id)->getFlippedImagePath() << endl;

		GLubyte* image = reader->getImageData();
		int imageWidth = reader->getWidth();
		int imageHeight = reader->getHeight();
		cout << "width: " << imageWidth << "   height: " << imageHeight << "   comp:  " << reader->getComponentCount() << endl;

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
	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		if (loc != -1)
		{
			float xTrans, yTrans;

			int objectCount = 0;
			vector<int> ids = scene.getIds();
			for (auto const& id : ids)
			{
				auto card = scene.get(id);
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
				
				vector<int> indexData = scene.getGeometry(0)->getIndexData();
				glUniform2f(loc, xTrans, yTrans);
				glDrawRangeElements(GL_TRIANGLES, 0, 5, 6, GL_UNSIGNED_INT, indexData.data());
			}

		}


		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glDeleteTextures(scene.size(), textureNames);
	delete[] textureNames;

	return 0;
}
