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
	grid->init(60, 0.1, 1);

	/* Create Camera and Set Projection Matrix */
	CAMERA = new Camera();
	CAMERA->init();

	/* BUILD & COMPILE SHADERS */
	cellShader = new Shader("../src/shaders/shader.vert", "../src/shaders/texShader.frag");
	gridShader = new Shader("../src/shaders/shader.vert", "../src/shaders/shader.frag");

	/*Misc*/
	glEnable(GL_DEPTH_TEST);
	glPointSize(4.f);

	/*Bind VBO, VAO*/
	bindVertices();

	/*Set Perspective Matrix*/
	changeScrDimensions(SCR_WIDTH, SCR_HEIGHT);
}

void Simulator::moveCamera(vec3 moveBy) {
	CAMERA->move(moveBy);
}

void Simulator::changeScrDimensions(int width, int height) {
	SCR_WIDTH = width;
	SCR_HEIGHT = height;

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	//Projection matrix (Camera to screen)
	glm::mat4 c2s;
	c2s = perspective(radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	gridShader->use();
	gridShader->setMat4("projection", c2s);
	cellShader->use();
	cellShader->setMat4("projection", c2s);
}

void Simulator::bindVertices() {

	/*****************/
	/* GRID VERTICES */
	/*****************/

	int gridWidth = grid->grid_size + 2;
	gridVBO = std::vector<GLuint> (gridWidth * gridWidth, 0);
	gridVAO = std::vector<GLuint> (gridWidth * gridWidth, 0);

	glGenBuffers(gridWidth * gridWidth, &gridVBO[0]);
	glGenVertexArrays(gridWidth * gridWidth, &gridVAO[0]);

	//for (int i = 0; i < vertices.size()/3; i++) {
	//	/* Bind VBOs and set VBO data */
	//	glBindBuffer(GL_ARRAY_BUFFER, (*VBO)[i]);
	//	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), &vertices[i * 3], GL_STATIC_DRAW);

	//	/* Bind VAO and set VAO configuration */
	//	glBindVertexArray((*VAO)[i]);
	//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
	//	glEnableVertexAttribArray(0);
	//}

	//int index = 0;
	std::vector<float> border = std::vector<float> (0);
	border.reserve(15);

	for (int j = 0; j < gridWidth; j++) {
		for (int i = 0; i < gridWidth; i++) {
			border.clear();
			//Bottom Left Corner
			border.push_back(grid->vertices[j * (gridWidth+1) + i].x);
			border.push_back(grid->vertices[j * (gridWidth + 1) + i].y);
			border.push_back(grid->vertices[j * (gridWidth + 1) + i].z);
			//Top Right Corner
			border.push_back(grid->vertices[(j + 1) * (gridWidth + 1) + i + 1].x);
			border.push_back(grid->vertices[(j + 1) * (gridWidth + 1) + i + 1].y);
			border.push_back(grid->vertices[(j + 1) * (gridWidth + 1) + i + 1].z);
			//Bottom Right Corner
			border.push_back(grid->vertices[j * (gridWidth + 1) + i + 1].x);
			border.push_back(grid->vertices[j * (gridWidth + 1) + i + 1].y);
			border.push_back(grid->vertices[j * (gridWidth + 1) + i + 1].z);
			//Bottom Left Corner
			border.push_back(grid->vertices[j * (gridWidth + 1) + i].x);
			border.push_back(grid->vertices[j * (gridWidth + 1) + i].y);
			border.push_back(grid->vertices[j * (gridWidth + 1) + i].z);
			//Top Left Corner
			border.push_back(grid->vertices[(j + 1) * (gridWidth + 1) + i].x);
			border.push_back(grid->vertices[(j + 1) * (gridWidth + 1) + i + 1].y);
			border.push_back(grid->vertices[(j + 1) * (gridWidth + 1) + i + 1].z);
			//Top Right Corner
			border.push_back(grid->vertices[(j + 1) * (gridWidth + 1) + i + 1].x);
			border.push_back(grid->vertices[(j + 1) * (gridWidth + 1) + i + 1].y);
			border.push_back(grid->vertices[(j + 1) * (gridWidth + 1) + i + 1].z);
			/* Bind */
			bindVertexSet(j*gridWidth + i, 18, border);
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

void Simulator::simulate(float time) {
	grid->calculateAdvection();
	grid->calculateDiffusion(20);
	drawContents();
}

void Simulator::drawContents() {

	/* CLEAR PREVIOUS */
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* Set View Matrix */

	//SAMPLE ROTATING CAMERA
	//float radius = 10.0f;
	//float camX = sin(glfwGetTime()) * radius;
	//float camZ = cos(glfwGetTime()) * radius;
	//mat4 view;
	//view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

	//View matrix
	glm::mat4 w2c;
	w2c = CAMERA->getViewMatrix();

	gridShader->use();
	gridShader->setMat4("view", w2c);

	cellShader->use();
	cellShader->setMat4("view", w2c);

	/* DRAW */
	drawArrows();
	drawGrid();
}

//Called in drawContents()
void Simulator::drawGrid() {

	//Model matrix (CALCULATED PER OBJECT)
	glm::mat4 o2w;
	o2w = scale(o2w, vec3(1.f, 1.f, 1.f));

	cellShader->use();
	cellShader->setMat4("model", o2w);
	gridShader->use();
	gridShader->setMat4("model", o2w);

	//glDrawArrays(GL_POINTS, 0, grid->vertices->size() - 3); //Parameters: Primitive to draw, range of vertex array to draw
	//glDrawArrays(GL_LINE_STRIP, 0, 3); //Parameters: Primitive to draw, range of vertex array to draw

	for (int i = 0; i < gridVAO.size(); i++) {
		glBindVertexArray(gridVAO[i]);
		//GRID ONLY
		gridShader->use();
		glDrawArrays(GL_LINE_LOOP, 1, 4);
		//CELL BGS
		cellShader->use();
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}
}

//Called in drawContents()
void Simulator::drawArrows() {

	gridShader->use();
	glBindVertexArray(velocityVAO);

	for (int i = 0; i < grid->centroid_vecs.size(); i++) {
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