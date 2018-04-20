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
	grid->init(1000, 0.01, 1);
	GPUsim = true;
	normalized_renders = true;
	current = VELOCITY;

	/* Create Camera and Set Projection Matrix */
	CAMERA = new Camera();
	CAMERA->init();

	/* BUILD & COMPILE SHADERS */
	cellShader = new Shader("../src/shaders/texShader.vert", "../src/shaders/texShader.frag");
	gridShader = new Shader("../src/shaders/shader.vert", "../src/shaders/shader.frag");

	/*Misc*/
	glEnable(GL_DEPTH_TEST);

	/*Bind VBO, VAO*/
	//bindVertices();
	bindScreenVertices();
	generateTextures();

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
	if (GPUsim) {
		grid->stepOnce(20);
		grid->extForces(time);
		grid->projectGPU(5);
		grid->moveDye(time);
	}
	else {
		grid->calculateAdvection();
		grid->calculateDiffusion(5);
		grid->project(5);
		grid->boundaryConditions();
	}
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

	/* DRAW */
	if (GPUsim) {
		glm::mat4 o2w;
		o2w = scale(o2w, vec3(1.f, 1.f, 1.f));

		cellShader->use();
		cellShader->setMat4("model", o2w);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, grid->renderToScreen);
		glBindVertexArray(screenVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	} else {
		//drawArrows();
		//drawGrid();
		drawTexture();
	}
}

/* CPU Vertices */
void Simulator::bindVertices() {

	/*****************/
	/* GRID VERTICES */
	/*****************/

	int gridWidth = grid->grid_size + 2;
	gridVBO = std::vector<GLuint>(gridWidth * gridWidth, 0);
	gridVAO = std::vector<GLuint>(gridWidth * gridWidth, 0);

	glGenBuffers(gridWidth * gridWidth, &gridVBO[0]);
	glGenVertexArrays(gridWidth * gridWidth, &gridVAO[0]);

	//int index = 0;
	std::vector<float> cell = std::vector<float>(0);
	cell.reserve(15);

	for (int j = 0; j < gridWidth; j++) {
		for (int i = 0; i < gridWidth; i++) {
			cell.clear();
			//Bottom Left Corner
			cell.push_back(grid->vertices[j * (gridWidth + 1) + i].x);
			cell.push_back(grid->vertices[j * (gridWidth + 1) + i].y);
			cell.push_back(grid->vertices[j * (gridWidth + 1) + i].z);
			//Top Right Corner
			cell.push_back(grid->vertices[(j + 1) * (gridWidth + 1) + i + 1].x);
			cell.push_back(grid->vertices[(j + 1) * (gridWidth + 1) + i + 1].y);
			cell.push_back(grid->vertices[(j + 1) * (gridWidth + 1) + i + 1].z);
			//Bottom Right Corner
			cell.push_back(grid->vertices[j * (gridWidth + 1) + i + 1].x);
			cell.push_back(grid->vertices[j * (gridWidth + 1) + i + 1].y);
			cell.push_back(grid->vertices[j * (gridWidth + 1) + i + 1].z);
			//Bottom Left Corner
			cell.push_back(grid->vertices[j * (gridWidth + 1) + i].x);
			cell.push_back(grid->vertices[j * (gridWidth + 1) + i].y);
			cell.push_back(grid->vertices[j * (gridWidth + 1) + i].z);
			//Top Left Corner
			cell.push_back(grid->vertices[(j + 1) * (gridWidth + 1) + i].x);
			cell.push_back(grid->vertices[(j + 1) * (gridWidth + 1) + i + 1].y);
			cell.push_back(grid->vertices[(j + 1) * (gridWidth + 1) + i + 1].z);
			//Top Right Corner
			cell.push_back(grid->vertices[(j + 1) * (gridWidth + 1) + i + 1].x);
			cell.push_back(grid->vertices[(j + 1) * (gridWidth + 1) + i + 1].y);
			cell.push_back(grid->vertices[(j + 1) * (gridWidth + 1) + i + 1].z);
			/* Bind */
			bindVertexSet(j*gridWidth + i, 18, cell);
		}
	}

	/*******************/
	/* Velocity Arrows */
	/*******************/

	//Create one arrow, which will be transformed for each grid cell
	float arrow[] = {
		0.f, 0.f, 0.0f,
		0.f, .6f, 0.0f
	};

	glGenBuffers(1, &velocityVBO);
	glGenVertexArrays(1, &velocityVAO);

	glBindBuffer(GL_ARRAY_BUFFER, velocityVBO);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), arrow, GL_STATIC_DRAW);

	glBindVertexArray(velocityVAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glEnableVertexAttribArray(0);

	/**********/
	/* Unbind */
	/**********/

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
void Simulator::bindVertexSet(unsigned int index, unsigned int size, const std::vector<float> vertices) {
	/* Bind VBOs and set VBO data */
	glBindBuffer(GL_ARRAY_BUFFER, gridVBO[index]);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	/* Bind VAO and set VAO configuration */
	glBindVertexArray((gridVAO)[index]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glEnableVertexAttribArray(0);
}

/* CPU Computation Textures */
void Simulator::generateTextures() {
	cellShader->use();
	FBO = new Framebuffer ();
	std::vector<float> velocities = std::vector<float>(0);
	for (unsigned int i = 0; i < grid->velocities.size(); i++) {
		vec3 velocity = grid->velocities[i];
		if (length(velocity) != 0) {
			velocity = normalize(velocity);
		}
		velocities.push_back((float)(velocity.x + 1.f) / 2.);
		velocities.push_back((float)(velocity.y + 1.f) / 2.);
		velocities.push_back((float)(velocity.z + 1.f) / 2.);
	}
	currentTex = FBO->createTexture(grid->grid_size + 2, &velocities[0]);
}
void Simulator::updateVelocityTexture(bool normalized) {
	std::vector<float> velocities = std::vector<float> (0);
	for (unsigned int i = 0; i < grid->velocities.size(); i++) {
		vec3 velocity = grid->velocities[i];
		if (normalized && length(velocity) != 0) {
			velocity = normalize(velocity);
		}
		velocities.push_back((float)(velocity.x + 1.f) / 2.);
		velocities.push_back((float)(velocity.y + 1.f) / 2.);
		velocities.push_back((float)(velocity.z + 1.f) / 2.);
	}
	FBO->changeTextureImage(grid->grid_size + 2, currentTex, &velocities[0]);
}
void Simulator::updatePressureTexture() {
	std::vector<float> pressures = std::vector<float>(0);
	for (unsigned int i = 0; i < grid->pressures.size(); i++) {
		pressures.push_back(grid->pressures[i]);
		pressures.push_back(0.f);
		pressures.push_back(0.f);
	}
	cellShader->use();
	FBO->changeTextureImage(grid->grid_size + 2, currentTex, &pressures[0]);
}
void Simulator::updateDivergenceTexture() {
	std::vector<float> divergences = std::vector<float>(0);
	for (unsigned int i = 0; i < grid->divergences.size(); i++) {
		divergences.push_back((float) grid->divergences[i]);
		divergences.push_back(0.f);
		divergences.push_back(0.f);
	}
	FBO->changeTextureImage(grid->grid_size + 2, currentTex, &divergences[0]);
}

/* CPU Render Calls */
void Simulator::drawTexture() {
	switch (current) {
	case VELOCITY:
		updateVelocityTexture(normalized_renders);
		break;
	case PRESSURE:
		updatePressureTexture();
		break;
	case DIVERGENCE:
		updateDivergenceTexture();
		break;
	case INK:
		break;
	}

	glm::mat4 o2w;
	o2w = scale(o2w, vec3(1.f, 1.f, 1.f));

	cellShader->use();
	cellShader->setMat4("model", o2w);
	glBindTexture(GL_TEXTURE_2D, currentTex);
	glBindVertexArray(screenVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
void Simulator::drawGrid() {

	//Model matrix (CALCULATED PER OBJECT)
	glm::mat4 o2w;
	o2w = scale(o2w, vec3(1.f, 1.f, 1.f));

	cellShader->use();
	cellShader->setMat4("model", o2w);
	gridShader->use();
	gridShader->setMat4("model", o2w);

	for (unsigned int i = 0; i < gridVAO.size(); i++) {
		glBindVertexArray(gridVAO[i]);
		//GRID ONLY
		gridShader->use();
		glDrawArrays(GL_LINE_LOOP, 1, 4);
		//CELL BGS
		//float red = grid->pressures[i] * .5f + .5f;
		//cellShader->use();
		//cellShader->setVec4("color", vec4(red, .3f, .3f, 1.f));
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
}
void Simulator::drawArrows() {

	gridShader->use();
	glBindVertexArray(velocityVAO);

	for (unsigned int i = 0; i < grid->centroid_vecs.size(); i++) {
		//Model matrix (CALCULATED PER OBJECT)
		vec3 velocity = grid->velocities[i];
		if (length(velocity) != 0) {
			velocity = normalize(velocity);
		}
		float angle = glm::angle(velocity, normalize(vec3(0.f, 1.f, 0.f)));
		if (velocity.x > 0) {
			angle = -angle;
		}

		glm::mat4 o2w;
		o2w = translate(o2w, grid->centroid_vecs[i]);
		o2w = scale(o2w, vec3(grid->cell_size/2.f, grid->cell_size/2.f, 1.f));
		o2w = rotate(o2w, angle, vec3(0.f, 0.f, 1.f));
		gridShader->setMat4("model", o2w);

		glDrawArrays(GL_LINES, 0, 2);
		glDrawArrays(GL_POINTS, 0, 2);
	}

	glBindVertexArray(0);
}