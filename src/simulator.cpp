#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
using namespace glm;
#include <vector>
#include <iostream>

#include "simulator.h"
#include "camera.h"

void Simulator::init() {

	/* Create new grid */
	grid = new Grid();
	grid->init(1000, 0.05, 1);

	/* Create Camera and Set Projection Matrix */
	CAMERA = new Camera();
	CAMERA->init();

	/* BUILD & COMPILE SHADERS */
	cellShader = new Shader("../src/shaders/texShader.vert", "../src/shaders/defaultShader.frag");
	gridShader = new Shader("../src/shaders/shader.vert", "../src/shaders/shader.frag");

	/*Misc*/
	glEnable(GL_DEPTH_TEST);

	bindScreenVertices();

	/*Set Perspective Matrix*/
	changeScrDimensions((int) SCR_WIDTH, (int) SCR_HEIGHT);
}

void Simulator::moveCamera(vec3 moveBy) {
	CAMERA->move(moveBy);
}

void Simulator::changeScrDimensions(int width, int height) {
	SCR_WIDTH = (float) width;
	SCR_HEIGHT = (float) height;

	glViewport(0, 0, (unsigned int) SCR_WIDTH, (unsigned int)SCR_HEIGHT);

	//Projection matrix (Camera to screen)
	glm::mat4 c2s;
	c2s = perspective(radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	gridShader->use();
	gridShader->setMat4("projection", c2s);
	cellShader->use();
	cellShader->setMat4("projection", c2s);
}

void Simulator::bindScreenVertices() {
	
	/* SCREEN VERTICES */
	float screen[] = {
		//Vertices       //Tex Coords
		-1.f, -1.f, 0.f,    0.f, 0.f,
		-1.f, 1.f, 0.f,    0.f, 1.f,
		1.f, 1.f, 0.f,    1.f, 1.f,

		-1.f, -1.f, 0.f,    0.f, 0.f,
		1.f, -1.f, 0.f,    1.f, 0.f,
		1.f, 1.f, 0.f,    1.f, 1.f,
	};

	glGenBuffers(1, &screenVBO);
	glGenVertexArrays(1, &screenVAO);

	/* Bind VBO and set VBO data */
	glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
	glBufferData(GL_ARRAY_BUFFER, 6 * 5 * sizeof(float), screen, GL_STATIC_DRAW);

	/* Bind VAO and set VAO configuration */
	glBindVertexArray(screenVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);

	/* Unbind */
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Simulator::simulate(float time) {
	grid->stepOnce(20);
	grid->extForces(time);
	grid->projectGPU(20);
	grid->moveDye(time);
	drawContents();
}

void Simulator::drawContents() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/* CLEAR PREVIOUS */
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	/* Set View Matrix */
	//View matrix
	glm::mat4 w2c;
	w2c = CAMERA->getViewMatrix();

	gridShader->use();
	gridShader->setMat4("view", w2c);

	cellShader->use();
	cellShader->setMat4("view", w2c);

	drawTexture();
}

void Simulator::drawTexture() {
	glm::mat4 o2w;
	o2w = scale(o2w, vec3(1.f, 1.f, 1.f));

	cellShader->use();
	cellShader->setMat4("model", o2w);
	glBindTexture(GL_TEXTURE_2D, grid->renderToScreen);
	glBindVertexArray(screenVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}