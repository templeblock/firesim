#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "simulator.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 950;
const unsigned int SCR_HEIGHT = 950;

Simulator *simulation;

float deltaTime = 0;
float lastFrame = 0;
double lastMousePosX;
double lastMousePosY;
bool drag = false;
bool rightDrag = false;


int main(void) {

	/**************************/
	/* Initialize GLFW & GLAD */
	/**************************/
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	/* CREATE WINDOW */
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL

	GLFWwindow* window; // (In the accompanying source code, this variable is global for simplicity)
	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "FireSim", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	/* SET CALLBACKS */
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	/*************************/
	/* Initialize Simulation */
	/*************************/

	simulation = new Simulator((float) SCR_WIDTH, (float)SCR_HEIGHT);
	simulation->init();

	/***************/
	/* RENDER LOOP */
	/***************/
	int frame = 0;

	while (!glfwWindowShouldClose(window)) {

		float currentFrame = (float) glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		frame++;

		/**Input**/
		processInput(window);

		double lastPosX = lastMousePosX;
		double lastPosY = lastMousePosY;
		glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);
		if (rightDrag) {
			simulation->rotateCamera(lastPosX - lastMousePosX, lastPosY - lastMousePosY);
		}

		/** Render **/
		simulation->simulate(currentFrame, vec2(lastMousePosX/SCR_WIDTH, -lastMousePosY / SCR_HEIGHT), drag);

		/**Swap buffers & poll IO events**/
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

/*Process input: query GLFW for keys*/
void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float camSpeed = 2.5f * deltaTime;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
		simulation->toggleFlameColor();
	if (key == GLFW_KEY_T && action == GLFW_PRESS)
		simulation->toggleTexture();
	if (key == GLFW_KEY_R && action == GLFW_PRESS)
		simulation->resetCamera();
	if (key == GLFW_KEY_S && action == GLFW_PRESS)
		simulation->toggleScene();
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	simulation->changeScrDimensions(width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	float camSpeed = 5.f * deltaTime;
	simulation->moveCamera(vec3(0.f, 0.f, camSpeed * yoffset));
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		drag = true;
		glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		drag = false;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		rightDrag = true;
		glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		rightDrag = false;
	}
}