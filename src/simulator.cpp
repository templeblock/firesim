#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <algorithm>
using namespace glm;
#include <vector>
#include <iostream>

#include "simulator.h"
#include "camera.h"

void Simulator::init() {
	FBO = new Framebuffer();

	fireColor = 0;

	/* Create new grid */
	grid = new Grid();
	grid->init(100, 0.1f, 1);
	resolution = 1000;

	/* Create Camera and Set Projection Matrix */
	CAMERA = new Camera();
	CAMERA->init(SCR_WIDTH, SCR_HEIGHT);

	/* BUILD & COMPILE SHADERS */
	cellShader = new Shader("../src/shaders/defaultShader.vert", "../src/shaders/defaultShader.frag");
	renderShader = new Shader("../src/shaders/shader.vert", "../src/shaders/renderShaderFlat.frag");
	renderShaderRay = new Shader("../src/shaders/texShader.vert", "../src/shaders/renderShader.frag");

	/*Misc*/
	glEnable(GL_DEPTH_TEST);
	std::vector<float> buffer = std::vector<float>(resolution * resolution * 3, 0.f);
	screenTex = FBO->createTexture(resolution, &buffer[0]);
	screenFBO = FBO->createFBO(screenTex);

	bindScreenVertices();

	/*Set Perspective Matrix*/
	changeScrDimensions((int) SCR_WIDTH, (int) SCR_HEIGHT);
	/*Set o2w values*/
	rotateX = 0; 
	rotateY = 0;
}

void Simulator::toggleFlameColor() {
	if (grid->renderToScreen == grid->fuelOutputTex) {
		fireColor++;
		if (fireColor > 3) {
			fireColor = 0;
		}
	}
}

void Simulator::toggleTexture() {
	if (grid->renderToScreen == grid->fuelOutputTex) {
		grid->renderToScreen = grid->velocityInputTex;
		fireColor = 4;
	}
	else {
		grid->renderToScreen = grid->fuelOutputTex;
		fireColor = 0;
	}
}

void Simulator::toggleScene() {
	if (grid->scene == 0) {
		grid->scene = 1;
	}
	else {
		grid->scene = 0;
	}
}

void Simulator::moveCamera(vec3 moveBy) {
	CAMERA->move(moveBy);
}

void Simulator::rotateCamera(double deltaX, double deltaY) {
	rotateX += deltaX/4.;
	rotateY += deltaY/4.;
	if (rotateY > 90)
		rotateY = 90;
	if (rotateY < -90)
		rotateY = 90;
}

void Simulator::resetCamera() {
	rotateX = 0;
	rotateY = 0;
}

void Simulator::changeScrDimensions(int width, int height) {
	SCR_WIDTH = (float) width;
	SCR_HEIGHT = (float) height;

	CAMERA->changeScreenDimens(SCR_WIDTH, SCR_HEIGHT);

	//Projection matrix (Camera to screen)
	glm::mat4 c2s;
	c2s = perspective(radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	cellShader->use();
	cellShader->setMat4("projection", c2s);
	renderShader->use();
	renderShader->setMat4("projection", c2s);
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

void Simulator::simulate(float time, vec2 mousepos, bool click) {


	grid->stepOnce(0);
	grid->extForces(time);
	grid->projectGPU(8);
	vec4 mousePos;

	if (click) {
		glm::mat4 c2s;
		c2s = perspective(radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		//c2s = inverse(c2s);
		glm::mat4 w2c;
		w2c = CAMERA->getViewMatrix();
		w2c = inverse(w2c);
		glm::mat4 o2w;
		o2w = translate(o2w, vec3(0.f, 0.f, 0.f));
		o2w = translate(o2w, vec3(0.f, 0.f, CAMERA->camPos.z - 3.f));
		glm::mat4 obj;
		obj = rotate(obj, radians(rotateY), vec3(1.f, 0.f, 0.f));
		obj = rotate(obj, radians(rotateX), vec3(0.f, 1.f, 0.f));
		obj = inverse(obj);
		mousePos = vec4(mousepos, 0.0f, 1.f) * 2.f;
		mousePos += vec4(-1.f, 1.f, 0.f, 0.f);
		mousePos = obj * mousePos;
	}

	grid->moveDye(time, mousePos, click);
	drawContents();
}

void Simulator::drawContents() {
	render();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/* CLEAR PREVIOUS */
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	cellShader->use();
	glBindTexture(GL_TEXTURE_2D, screenTex);
	glBindVertexArray(screenVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void Simulator::render() {
	glViewport(0, 0, resolution, resolution);
	glBindVertexArray(screenVAO);
	glBindFramebuffer(GL_FRAMEBUFFER, screenFBO);
	/* CLEAR PREVIOUS */
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Set View Matrix */
	//View matrix
	glm::mat4 w2c;
	w2c = CAMERA->getViewMatrix();
	glm::mat4 o2w;
	o2w = translate(o2w, vec3(0.f, 0.f, 0.f));
	glm::mat4 obj;
	obj = rotate(obj, radians(rotateY), vec3(1.f, 0.f, 0.f));
	obj = rotate(obj, radians(rotateX), vec3(0.f, 1.f, 0.f));

	//renderShader->use();
	//renderShader->setMat4("view", w2c);
	//renderShader->setMat4("model", o2w);
	//renderShader->setMat4("object", obj);
	//renderShader->setFloat("cellSize", grid->cell_size/2.f);
	//renderShader->setInt("fireColor", fireColor);


	o2w = translate(o2w, vec3(0.f, 0.f, CAMERA->camPos.z - 3.f));
	renderShaderRay->use();
	renderShaderRay->setMat4("view", w2c);
	renderShaderRay->setVec4("camPos", vec4(CAMERA->camPos, 1.f));
	renderShaderRay->setMat4("model", o2w);
	renderShaderRay->setMat4("object", obj);
	renderShaderRay->setFloat("cellSize", grid->cell_size / 2.f);
	renderShaderRay->setInt("fireColor", fireColor);



	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, grid->renderToScreen);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}