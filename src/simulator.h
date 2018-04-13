#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <vector>

#include "camera.h"
#include "shader.h"
#include "grid.h"

class Simulator {
	
public:

	Simulator(float width, float height) {
		SCR_HEIGHT = height;
		SCR_WIDTH = width;
	}
	void init();

	void bindVertices();
	void bindVertexSet(unsigned int index, unsigned int pointer, const std::vector<float> vertices);

	void drawContents();
	void drawGrid();

	void drawArrows();

	void moveCamera(vec3 moveBy);

	void changeScrDimensions(int width, int height);

private:
	std::vector<GLuint> gridVBO;
	std::vector<GLuint> gridVAO;

	//std::vector<GLuint> *velocityVBO;
	//std::vector<GLuint> *velocityVAO;

	GLuint velocityVBO;
	GLuint velocityVAO;

	Camera *CAMERA;
	Shader *gridShader;
	Shader *cellShader;
	Grid *grid;
	
	float SCR_HEIGHT;
	float SCR_WIDTH;
};

#endif