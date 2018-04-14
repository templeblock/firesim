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

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

Simulator *simulation;

float deltaTime = 0;
float lastFrame = 0;

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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "Failed to initialize GLAD" << endl;
		return -1;
	}

	/*************************/
	/* Initialize Simulation */
	/*************************/

	simulation = new Simulator(SCR_WIDTH, SCR_HEIGHT);
	simulation->init();

	/***************/
	/* RENDER LOOP */
	/***************/

	while (!glfwWindowShouldClose(window)) {

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		/**Input**/
		processInput(window);

		/** Render **/
		simulation->simulate(deltaTime);

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