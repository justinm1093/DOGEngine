
#include "pch.h"

#include "glfw3.h"

static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	UNREFERENCED_PARAMETER(scancode);
	UNREFERENCED_PARAMETER(mods);

	// really simple method detects when the escape key is pressed
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		// set window close flag
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// avoids unreferenced param warning which would cause compilation error
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	// initialize glfw library
	if(glfwInit())
	{
		// create window
		GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL Window", NULL, NULL);
		if(window)
		{
			// set window as current context
			glfwMakeContextCurrent(window);
			glfwSetWindowShouldClose(window, GLFW_FALSE);

			// set window's callback function for key presses
			glfwSetKeyCallback(window, keyCallback);

			// event loop
			while(!glfwWindowShouldClose(window))
			{
				// check for events
				glfwPollEvents();
			}

			// cleanup, destroy window
			glfwDestroyWindow(window);
		}

		// cleanup, shutdown glfw library
		glfwTerminate();
	}

	return 0;
}
