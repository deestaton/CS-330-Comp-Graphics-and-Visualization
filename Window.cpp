#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}

	// Prevent the camera from starting off facing a random direction by setting x and y change values to 0
	xChange = 0.0f;
	yChange = 0.0f;
}

Window::Window(GLfloat windowWidth, GLfloat windowHeight)
{
	width = windowWidth;
	height = windowHeight;

	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}

	// Prevent the camera from starting off facing a random direction by setting x and y change values to 0
	xChange = 0.0f;
	yChange = 0.0f;
}

int Window::Initialize()
{
	if (!glfwInit())
	{
		printf("Error Initializing GLFW");
		glfwTerminate();
		return -1;
	}

	// Setup GLFW window properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create the window
	mainWindow = glfwCreateWindow(width, height, "CS-330 OpenGL Project | Dewayne Staton", NULL, NULL);
	if (!mainWindow)
	{
		printf("Failed to open GLFW window");
		glfwTerminate();
		return -1;
	}

	// Get buffer size information
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set the current context
	glfwMakeContextCurrent(mainWindow);

	// Handle Key + Mouse Input
	createCallbacks();

	// Lock the cursor to the window
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Allow modern extension access
	glewExperimental = GL_TRUE;

	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		printf("Error: %s", glewGetErrorString(error));
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return -1;
	}

	glEnable(GL_DEPTH_TEST); // determines which triangle to draw over others

	// Create viewport
	glViewport(0, 0, bufferWidth, bufferHeight);

	// Sets a user-defined pointer for the specified window
	// Useful for passing the window object to callbacks, or storing associated with a window.
	glfwSetWindowUserPointer(mainWindow, this);
}


void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, handleKeys);
	glfwSetCursorPosCallback(mainWindow, handleMouse);
}

// Reset the xChange and yChange values back to zero if no movement
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) // Checks if the escape key is pressed/releaed to close the window
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024) // ensures the key is a valid key
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			printf("Pressed: %d\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			printf("Released: %d\n", key);
		}
	}
}

// When the mouse is moved in the main window, callback to the handleMouse() function
void Window::handleMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos; // Switch this around if you prefer inverted or non-inverted on the y-axis
	
	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();
}