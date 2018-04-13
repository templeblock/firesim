#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
#include <vector>;

#include "simulator.h"
#include "camera.h"

void Simulator::init() {

	/* Create new grid */
	grid = new Grid();
	grid->init();

	/* BUILD & COMPILE SHADERS */
	shader = new Shader("../src/shaders/shader.vert", "../src/shaders/shader.frag");
	shader->use();

	/* Create Camera and Set Projection Matrix */
	CAMERA = new Camera();
	CAMERA->init();

	//Projection matrix (Camera to screen)
	glm::mat4 c2s;
	c2s = perspective(radians(60.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	shader->setMat4("projection", c2s);
	glEnable(GL_DEPTH_TEST);

	/*Bind VBO, VAO*/
	bindVertices();
}

void Simulator::moveCamera(vec3 moveBy) {
	CAMERA->move(moveBy);
}

void Simulator::bindVertices(const std::vector<vec3> *vertices) {
	/***********************************/
	/* VERTEX DATA, ATTRIBUTES; BUFFERS*/
	/***********************************/

	//HARD-CODED SAMPLE VERTICES
	//float vertices[] = {
	//	-0.5f, -0.5f, -1.f,
	//	0.5f, -0.5f, -1.f,
	//	0.0f,  0.5f, -1.f
	//};

	//float cube[] = {
	//	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	//	0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	//	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	//	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	//	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	//	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	//	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	//	0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	//	0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	//	0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	//	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	//	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	//	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	//	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	//	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	//	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	//	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	//	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	//	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	//	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	//	0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	//	0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	//	0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	//	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	//	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	//	0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	//	0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	//	0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	//	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	//	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	//	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	//	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	//	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	//	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	//	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	//	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	//};

	/*Generate VBO and VAO*/
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);

	/* Bind VBO and set VBO data */
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices->size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	/* Bind VAO and set VAO configuration */
	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*) 0);
	glEnableVertexAttribArray(0);

	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);
}

void Simulator::drawContents() {
	/* Set Transform Matrices */

	//Calculate Transform Matrices
	//Model matrix (CALCULATED PER OBJECT)
	glm::mat4 o2w;
	//o2w = rotate(o2w, glm::radians(55.f), glm::vec3(1.0f, 0.3f, 0.5f));
	o2w = scale(o2w, vec3(0.8f, 0.8f, 1.f));

	//View matrix
	glm::mat4 w2c;
	w2c = CAMERA->getViewMatrix();

	//SAMPLE ROTATING CAMERA
	//float radius = 10.0f;
	//float camX = sin(glfwGetTime()) * radius;
	//float camZ = cos(glfwGetTime()) * radius;
	//mat4 view;
	//view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));

	//Set transform matrices in shader
	shader->setMat4("model", o2w);
	shader->setMat4("view", w2c);

	/* DRAW */
	glBindVertexArray(VAO); //If switching objects, rebind different VAO
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	glDrawArrays(GL_POINTS, 0, 9); //Parameters: Primitive to draw, range of vertex array to draw
}