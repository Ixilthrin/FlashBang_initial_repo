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
#include "Converter.h"

#include "ImageReader.h"
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
	window = glfwCreateWindow(width, height, "Select and Move Multiple Objects Prototype", NULL, NULL);
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

	Converter converter{ width, height };

	float x1 = converter.screenXToNDC(0);
	float y1 = converter.screenYToNDC(0);
	float x2 = converter.screenXToNDC(scene.get(0)->getWidth());
	float y2 = converter.screenYToNDC(scene.get(0)->getHeight());

	float positionData[] =
	{
		x1,
		y1,
		0.0f,

		x1,
		y2,
		0.0f,

		x2,
		y2,
		0.0f,

		x2,
		y1,
		0.0f
	};

	float colorData[] =
	{
		.82f, 0.41f, 0.14f,
		.82f, 0.41f, 0.14f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f
	};

	float texCoords[] =
	{
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f
	};

	int positionLength = sizeof(positionData) / sizeof(positionData[0]);
	int colorDataLength = sizeof(colorData) / sizeof(colorData[0]);
	int texCoordLength = sizeof(texCoords) / sizeof(texCoords[0]);

	GLuint vboHandles[3];
	glGenBuffers(3, vboHandles);

	GLuint positionBufferHandle = vboHandles[0];
	GLuint colorBufferHandle = vboHandles[1];
	GLuint texCoordsBufferHandle = vboHandles[2];

	glBindBuffer(GL_ARRAY_BUFFER, positionBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, positionLength * sizeof(float), positionData, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, colorDataLength * sizeof(float), colorData, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, texCoordsBufferHandle);
	glBufferData(GL_ARRAY_BUFFER, texCoordLength * sizeof(float), texCoords, GL_STATIC_DRAW);


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

		ImageReader reader(scene.get(id)->getImagePath());

		GLubyte* image = reader.getImageData();
		int imageWidth = reader.getWidth();
		int imageHeight = reader.getHeight();
		cout << "width: " << imageWidth << "   height: " << imageHeight << "   comp:  " << reader.getComponentCount() << endl;

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

		ImageReader reader(scene.get(id)->getFlippedImagePath());
		cout << "flipped image path: " << scene.get(id)->getFlippedImagePath() << endl;

		GLubyte* image = reader.getImageData();
		int imageWidth = reader.getWidth();
		int imageHeight = reader.getHeight();
		cout << "width: " << imageWidth << "   height: " << imageHeight << "   comp:  " << reader.getComponentCount() << endl;

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

				vector<int> indexData = scene.get(0)->getIndexData();
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
