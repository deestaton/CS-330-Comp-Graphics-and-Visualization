
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window();

	Window(GLfloat windowWidth, GLfloat windowHeight);

	int Initialize();

	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }

	bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }

	bool* getsKeys() { return keys;  }

	// Reset change values to zero if no movement
	GLfloat getXChange();
	GLfloat getYChange();

	void swapBuffers() { glfwSwapBuffers(mainWindow); }

	~Window();

private:
	GLFWwindow* mainWindow;

	GLint width, height;

	GLint bufferWidth, bufferHeight;

	bool keys[1024]; // Covers the range of ASCII characters (everything on the keyboard)

	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	bool mouseFirstMoved;

	void createCallbacks();

	// Only key and action will be used
	// key is the actual value of the key (between 0 and 1024)
	// action is the action performed on that key (press or release of the key)
	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);

	static void handleMouse(GLFWwindow* window, double xPos, double yPos);
};

