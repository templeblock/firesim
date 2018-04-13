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
	grid->init(15);

	/* Create Camera and Set Projection Matrix */
	CAMERA = new Camera();
	CAMERA->init();

	/* BUILD & COMPILE SHADERS */
	shader = new Shader("../src/shaders/shader.vert", "../src/shaders/shader.frag");
	shader->use();

	//Projection matrix (Camera to screen)
	glm::mat4 c2s;
	c2s = perspective(radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	//c2s = ortho(0.f, SCR_WIDTH, 0.f, SCR_HEIGHT, 0.1f, 100.f);
	shader->setMat4("projection", c2s);
	glEnable(GL_DEPTH_TEST);

	/*Misc*/
	glPointSize(5.f);

	/*Bind VBO, VAO*/
	bindVertices();
}

void Simulator::moveCamera(vec3 moveBy) {
	CAMERA->move(moveBy);
}

void Simulator::bindVertices() {

	/*****************/
	/* GRID VERTICES */
	/*****************/

	int gridWidth = grid->grid_size;

	gridVBO = new std::vector<GLuint>;
	gridVAO = new std::vector<GLuint>;

	gridVBO->resize(gridWidth * gridWidth * 2 + gridWidth + gridWidth + 1);
	gridVAO->resize(gridWidth * gridWidth * 2 + gridWidth + gridWidth + 1);

	glGenBuffers(gridWidth * gridWidth * 2 + gridWidth + gridWidth + 1, &(*gridVBO)[0]);
	glGenVertexArrays(gridWidth * gridWidth * 2 + gridWidth + gridWidth + 1, &(*gridVAO)[0]);

	//for (int i = 0; i < vertices.size()/3; i++) {
	//	/* Bind VBOs and set VBO data */
	//	glBindBuffer(GL_ARRAY_BUFFER, (*VBO)[i]);
	//	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), &vertices[i * 3], GL_STATIC_DRAW);

	//	/* Bind VAO and set VAO configuration */
	//	glBindVertexArray((*VAO)[i]);
	//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float)*3, (void*)0);
	//	glEnableVertexAttribArray(0);
	//}

	int index = 0;
	std::vector<float> *vertices = grid->vertices;

	std::vector<float> *border;
	border = new std::vector<float>;

	for (int i = 0; i < gridWidth; i++) {

		for (int j = 0; j < gridWidth; j++) {
			//BOTTOM OF CELL
			bindVertexSet(index, i * (gridWidth + 1) * 3 + (j*3), *(grid->vertices));
			index++;

			//LEFT OF CELL
			border->clear();
			border->push_back ((*vertices)[i * (gridWidth+1) * 3 + (j * 3)]);
			border->push_back ((*vertices)[(i * (gridWidth + 1) * 3) + 1 + (j * 3)]);
			border->push_back ((*vertices)[(i * (gridWidth + 1) * 3) + 2 + (j * 3)]);
			border->push_back ((*vertices)[(i + 1)*(gridWidth + 1) * 3 + (j * 3)]);
			border->push_back ((*vertices)[(i + 1)*(gridWidth + 1) * 3 + 1 + (j * 3)]);
			border->push_back ((*vertices)[(i + 1)*(gridWidth + 1) * 3 + 2 + (j * 3)]);
			bindVertexSet(index, 0, *border);
			index++;
		}

		//RIGHT GRID BORDER
		border->clear();
		border->push_back ((*vertices)[i * (gridWidth+1) * 3 + (gridWidth * 3)]);
		border->push_back ((*vertices)[(i * (gridWidth + 1) * 3) + 1 + (gridWidth * 3)]);
		border->push_back ((*vertices)[(i * (gridWidth + 1) * 3) + 2 + (gridWidth * 3)]);
		border->push_back ((*vertices)[(i + 1)*(gridWidth + 1) * 3 + (gridWidth * 3)]);
		border->push_back ((*vertices)[(i + 1)*(gridWidth + 1) * 3 + 1 + (gridWidth * 3)]);
		border->push_back ((*vertices)[(i + 1)*(gridWidth + 1) * 3 + 2 + (gridWidth * 3)]);
		bindVertexSet(index, 0, *border);
		index++;
	}
 
	for (int j = 0; j < gridWidth; j++) {
		//TOP OF GRID
		bindVertexSet(index, gridWidth * (gridWidth + 1) * 3 + (j * 3), *(grid->vertices));
		index++;
	}

	/*******************/
	/* Velocity Arrows */
	/*******************/

	//Create one arrow, which is transformed for each grid cell
	float arrow[] = {
		0.f, 0.f, 0.f,
		0.f, .6f, 0.f
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

void Simulator::bindVertexSet(unsigned int index, unsigned int pointer, const std::vector<float> vertices) {
	/* Bind VBOs and set VBO data */
	glBindBuffer(GL_ARRAY_BUFFER, (*gridVBO)[index]);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), &vertices[pointer], GL_STATIC_DRAW);

	/* Bind VAO and set VAO configuration */
	glBindVertexArray((*gridVAO)[index]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	glEnableVertexAttribArray(0);
}

void Simulator::drawContents() {
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
	shader->setMat4("view", w2c);


	/* DRAW */
	drawGrid();
	drawArrows();
}

//Called in drawContents()
void Simulator::drawGrid() {

	//Model matrix (CALCULATED PER OBJECT)
	glm::mat4 o2w;
	o2w = scale(o2w, vec3(1.f, 1.f, 1.f));
	shader->setMat4("model", o2w);

	//glDrawArrays(GL_POINTS, 0, grid->vertices->size() - 3); //Parameters: Primitive to draw, range of vertex array to draw
	//glDrawArrays(GL_LINE_STRIP, 0, 3); //Parameters: Primitive to draw, range of vertex array to draw

	for (int i = 0; i < gridVAO->size(); i++) {
		glBindVertexArray((*gridVAO)[i]);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
	}
}

//Called in drawContents()
void Simulator::drawArrows() {

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
		shader->setMat4("model", o2w);

		glDrawArrays(GL_LINES, 0, 2);
		glDrawArrays(GL_POINTS, 0, 2);
	}

	glBindVertexArray(0);
}