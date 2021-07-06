#include "BasicCardDeck.h"

#include <iostream>
#include <glad/glad.h>
#include <stdio.h>
#include <sstream>
#include <fstream>

using std::endl;
using std::cout;
using std::cin;
using std::cerr;
using std::string;
using std::ifstream;
using std::stringstream;

#include <map>

#include "DispatchingMouseHandlers.h" // Interesting: putting this after next line causes link errors
#include <GLFW/glfw3.h>

#include "Scene.h"
#include "Rectangle.h"
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

	int width = 1200;
	int height = 800;
	window = glfwCreateWindow(width, height, "Select and Move Multiple Objects Prototype", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -2;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGL())
		return -3;

	Scene scene;

	ImageReader imageReader1("c:/programming/FlashBangProject/resources/test.png");

	Rectangle rectangle1{ 200, 100, 20, 50, &imageReader1,
		"c:/programming/FlashBang/shaders/translation_and_texture.vert.glsl",
		"c:/programming/FlashBang/shaders/texture.frag.glsl"
	};

	scene.add(0, &rectangle1);

	ImageReader imageReader2("c:/programming/FlashBangProject/resources/stanleykubrick.png");

	Rectangle rectangle2{ 200, 100, 24, 54, &imageReader2,
		"c:/programming/FlashBang/shaders/translation_and_texture.vert.glsl",
		"c:/programming/FlashBang/shaders/texture.frag.glsl"
	};

	scene.add(1, &rectangle2);

	ImageReader imageReader3("c:/programming/FlashBangProject/resources/pkd.png");

	Rectangle rectangle3{ 200, 100, 28, 58, &imageReader3,
		"c:/programming/FlashBang/shaders/translation_and_texture.vert.glsl",
		"c:/programming/FlashBang/shaders/texture.frag.glsl"
	};

	scene.add(2, &rectangle3);

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

	glClearColor(0, 0, 0, 1);
	glPointSize(5);
	glLineWidth(3);

	GLuint textureNames[3];
	glCreateTextures(GL_TEXTURE_2D, 3, textureNames);

	std::map<int, GLuint> textures;
	auto ids = scene.getIds();
	int currentTexture = 0;
	for (auto id = ids.begin(); id < ids.end(); id++) 
	{
		std::cout << "drawing id: " << *id << std::endl;
		textures.insert(std::pair<int, GLuint>(*id, textureNames[currentTexture]));

		GLubyte* image = scene.get(*id)->getImage()->getImageData();
		int imageWidth = scene.get(*id)->getImage()->getWidth();
		int imageHeight = scene.get(*id)->getImage()->getHeight();
		std::cout << "width: " << imageWidth << "   height: " << imageHeight << std::endl;

		glTextureStorage2D(textureNames[currentTexture], 1, GL_RGB8, imageWidth, imageHeight);
		glBindTexture(GL_TEXTURE_2D, textureNames[currentTexture]);

		if (image == nullptr)
			throw(std::string("Failed to load image"));
		glTextureSubImage2D(textureNames[currentTexture], 0, 0, 0, imageWidth, imageHeight, GL_RGB, GL_UNSIGNED_BYTE, image);


		++currentTexture;
	}

	EventTranslator translator;

	DispatchingMouseHandlers::translator = &translator;
	translator.registerListener(&listener);
	glfwSetCursorPosCallback(window, DispatchingMouseHandlers::cursor_position_callback);
	glfwSetCursorEnterCallback(window, DispatchingMouseHandlers::cursor_enter_callback);
	glfwSetMouseButtonCallback(window, DispatchingMouseHandlers::mouse_button_callback);
	glfwSetScrollCallback(window, DispatchingMouseHandlers::scroll_callback);


	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(vaoHandle);

		if (loc != -1)
		{
			float xTrans, yTrans;

			std::vector<int> items = scene.getIds();
			int objectCount = 0;
			for (auto i = items.begin(); i < items.end(); i++)
			{
				glBindTextureUnit(0, textures[*i]);
				int selected = listener.getSelectedId();
				if (listener.isSelectAndMoveInProgress() && selected == *i)
				{
					xTrans = converter.screenTranslationXToNDC(scene.get(selected)->getTranslationX() + listener.getMovementX());
					yTrans = converter.screenTranslationYToNDC(scene.get(selected)->getTranslationY() + listener.getMovementY());
				}
				else
				{
					xTrans = converter.screenTranslationXToNDC(scene.get(*i)->getTranslationX());
					yTrans = converter.screenTranslationYToNDC(scene.get(*i)->getTranslationY());
				}

				std::vector<int> indexData = scene.get(0)->getIndexData();
				glUniform2f(loc, xTrans, yTrans);
				glDrawRangeElements(GL_TRIANGLES, 0, 5, 6, GL_UNSIGNED_INT, indexData.data());
				//glDrawRangeElements(GL_TRIANGLES, 0, 5, 3, GL_UNSIGNED_INT, indexData.data());
			}

		}


		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glDeleteTextures(1, textureNames);

	return 0;
}
