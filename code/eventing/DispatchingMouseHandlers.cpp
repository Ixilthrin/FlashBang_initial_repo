#include "DispatchingMouseHandlers.h"

#include <GLFW/glfw3.h>

#include <iostream>

EventTranslator *DispatchingMouseHandlers::translator = 0;

void DispatchingMouseHandlers::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (translator)
	{
		translator->mouseMoved(xpos, ypos);
	}
}

void DispatchingMouseHandlers::cursor_enter_callback(GLFWwindow* window, int entered)
{
	if (entered)
	{
		//std::cout << "Hello Mouse" << std::endl;
	}
	else
	{
		//std::cout << "Goodbye Mouse" << std::endl;
	}
}
void DispatchingMouseHandlers::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
		if (translator)
		{
			translator->translateMouseEvent(EventTranslator::EventType::LeftMouseDown);
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
	{
		translator->translateMouseEvent(EventTranslator::EventType::LeftMouseUp);
	}

}
void DispatchingMouseHandlers::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//std::cout << "Scrolling" << std::endl;
}